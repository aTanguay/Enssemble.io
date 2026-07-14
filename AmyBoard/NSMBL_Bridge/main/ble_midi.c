#include "ble_midi.h"
#include "config.h"
#include "bridge_state.h"

#include "esp_log.h"
#include "esp_nimble_hci.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/ble_gap.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"

static const char *TAG = "ble_midi";

// BLE MIDI Service UUID: 03B80E5A-EDE8-4B33-A751-6CE34EC4C700
static const ble_uuid128_t midi_svc_uuid =
    BLE_UUID128_INIT(0x00, 0xc7, 0xc4, 0x4e, 0xe3, 0x6c, 0x51, 0xa7,
                     0x33, 0x4b, 0xe8, 0xed, 0x5a, 0x0e, 0xb8, 0x03);

// BLE MIDI Characteristic UUID: 7772E5DB-3868-4112-A1A9-F2669D106BF3
static const ble_uuid128_t midi_char_uuid =
    BLE_UUID128_INIT(0xf3, 0x6b, 0x10, 0x9d, 0x66, 0xf2, 0xa9, 0xa1,
                     0x12, 0x41, 0x68, 0x38, 0xdb, 0xe5, 0x72, 0x77);

static QueueHandle_t s_midi_queue;
static uint16_t s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
static uint16_t s_midi_attr_handle;
static bool s_connected = false;

bool ble_midi_is_connected(void)
{
    return s_connected;
}

static int data_bytes_for_status(uint8_t status)
{
    switch (status & 0xF0) {
    case 0x80: return 2; // Note Off
    case 0x90: return 2; // Note On
    case 0xA0: return 2; // Poly Aftertouch
    case 0xB0: return 2; // CC
    case 0xC0: return 1; // Program Change
    case 0xD0: return 1; // Channel Aftertouch
    case 0xE0: return 2; // Pitch Bend
    default:   return -1;
    }
}

static void parse_midi_message(const uint8_t *data, uint16_t len)
{
    if (len < 3) return;

    ESP_LOGD(TAG, "BLE MIDI rx [%d bytes]", len);
    ESP_LOG_BUFFER_HEX_LEVEL(TAG, data, len, ESP_LOG_DEBUG);

    uint8_t running_status = 0;
    int i = 1; // skip header byte

    while (i < len) {
        // A message boundary starts with either:
        //   - a timestamp byte (MSB set), optionally followed by a status byte, or
        //   - a data byte (MSB clear) that continues the previous running status.
        if (data[i] & 0x80) {
            i++; // consume timestamp
            if (i >= len) break;

            // A status byte may follow the timestamp.
            if (data[i] & 0x80) {
                if (data[i] >= 0xF8) {
                    // System Real-Time — single byte, no data, no effect on running status
                    i++;
                    continue;
                }
                if (data[i] >= 0xF0) {
                    // System Common / SysEx — skip its data bytes, cancel running status
                    i++;
                    while (i < len && !(data[i] & 0x80)) i++;
                    running_status = 0;
                    continue;
                }
                running_status = data[i++]; // channel voice status
            }
            // else: timestamp immediately followed by running-status data
        }
        // else: running-status data with no timestamp — reuse running_status

        if (running_status == 0) {
            // Stray data byte with no established status — skip it
            i++;
            continue;
        }
        if (i >= len) break;

        int need = data_bytes_for_status(running_status);
        if (need < 0 || i + need > len) break;

        uint8_t msg_type = running_status & 0xF0;
        uint8_t ch0     = running_status & 0x0F;         // 0-indexed channel
        uint8_t channel = ch0 + 1;                       // 1-indexed for event

        // Bridge input filter (runtime, set by the UI knob): 0 = forward all
        // channels, else only the selected channel.
        uint8_t in_filter = bridge_in();
        if (in_filter != BRIDGE_ALL && channel != in_filter) {
            i += need;
            continue;
        }

        midi_event_t event = {0};
        event.channel = channel;

        switch (msg_type) {
        case 0x90:
            event.type = (data[i + 1] == 0) ? MIDI_NOTE_OFF : MIDI_NOTE_ON;
            event.data1 = data[i];
            event.data2 = data[i + 1];
            break;
        case 0x80:
            event.type = MIDI_NOTE_OFF;
            event.data1 = data[i];
            event.data2 = data[i + 1];
            break;
        case 0xB0:
            event.type = MIDI_CC;
            event.data1 = data[i];
            event.data2 = data[i + 1];
            ESP_LOGD(TAG, "CC %d = %d (ch %d)", data[i], data[i + 1], channel);
            break;
        case 0xC0:
            event.type = MIDI_PROGRAM_CHANGE;
            event.data1 = data[i];
            ESP_LOGI(TAG, "PC %d (ch %d)", data[i], channel);
            break;
        case 0xE0:
            event.type = MIDI_PITCH_BEND;
            event.pitch_bend = (int16_t)((data[i + 1] << 7) | data[i]) - 8192;
            break;
        case 0xD0:
            event.type = MIDI_AFTERTOUCH;
            event.data1 = data[i];
            break;
        default:
            i += need;
            continue;
        }

        i += need;
        bridge_note_in(event.type, event.data1, event.data2);  // IN activity/monitor
        xQueueSend(s_midi_queue, &event, 0);
    }
}

static int midi_chr_access(uint16_t conn_handle, uint16_t attr_handle,
                           struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        uint16_t len = OS_MBUF_PKTLEN(ctxt->om);
        uint8_t buf[128];
        if (len > sizeof(buf)) len = sizeof(buf);
        os_mbuf_copydata(ctxt->om, 0, len, buf);
        parse_midi_message(buf, len);
    }
    return 0;
}

static const struct ble_gatt_svc_def gatt_svcs[] = {
    {
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &midi_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]){
            {
                .uuid = &midi_char_uuid.u,
                .access_cb = midi_chr_access,
                .val_handle = &s_midi_attr_handle,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_WRITE_NO_RSP | BLE_GATT_CHR_F_NOTIFY,
            },
            {0},
        },
    },
    {0},
};

static int gap_event_handler(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
    case BLE_GAP_EVENT_CONNECT:
        if (event->connect.status == 0) {
            s_conn_handle = event->connect.conn_handle;
            s_connected = true;
            ESP_LOGI(TAG, "Connected");
        } else {
            s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
            s_connected = false;
            ESP_LOGI(TAG, "Connection failed, re-advertising");
            ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER,
                              &(struct ble_gap_adv_params){.conn_mode = BLE_GAP_CONN_MODE_UND,
                                                           .disc_mode = BLE_GAP_DISC_MODE_GEN},
                              gap_event_handler, NULL);
        }
        break;

    case BLE_GAP_EVENT_DISCONNECT:
        s_conn_handle = BLE_HS_CONN_HANDLE_NONE;
        s_connected = false;
        ESP_LOGI(TAG, "Disconnected, re-advertising");
        ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER,
                          &(struct ble_gap_adv_params){.conn_mode = BLE_GAP_CONN_MODE_UND,
                                                       .disc_mode = BLE_GAP_DISC_MODE_GEN},
                          gap_event_handler, NULL);
        break;

    default:
        break;
    }
    return 0;
}

static void start_advertising(void)
{
    struct ble_gap_adv_params adv_params = {
        .conn_mode = BLE_GAP_CONN_MODE_UND,
        .disc_mode = BLE_GAP_DISC_MODE_GEN,
    };

    struct ble_hs_adv_fields fields = {0};
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.name = (uint8_t *)DEVICE_NAME;
    fields.name_len = strlen(DEVICE_NAME);
    fields.name_is_complete = 1;
    ble_gap_adv_set_fields(&fields);

    struct ble_hs_adv_fields rsp_fields = {0};
    rsp_fields.uuids128 = (ble_uuid128_t[]){midi_svc_uuid};
    rsp_fields.num_uuids128 = 1;
    rsp_fields.uuids128_is_complete = 1;
    ble_gap_adv_rsp_set_fields(&rsp_fields);

    ble_gap_adv_start(BLE_OWN_ADDR_PUBLIC, NULL, BLE_HS_FOREVER,
                      &adv_params, gap_event_handler, NULL);

    ESP_LOGI(TAG, "Advertising as \"%s\"", DEVICE_NAME);
}

static void on_sync(void)
{
    uint8_t addr_type;
    ble_hs_id_infer_auto(0, &addr_type);
    start_advertising();
}

static void nimble_host_task(void *param)
{
    nimble_port_run();
    nimble_port_freertos_deinit();
}

void ble_midi_init(QueueHandle_t midi_queue)
{
    s_midi_queue = midi_queue;

    esp_err_t ret = nimble_port_init();
    ESP_ERROR_CHECK(ret);

    ble_svc_gap_device_name_set(DEVICE_NAME);
    ble_svc_gap_init();
    ble_svc_gatt_init();

    ble_gatts_count_cfg(gatt_svcs);
    ble_gatts_add_svcs(gatt_svcs);

    ble_hs_cfg.sync_cb = on_sync;

    nimble_port_freertos_init(nimble_host_task);

    ESP_LOGI(TAG, "BLE MIDI initialized");
}
