#include "sample_player.h"
#include "config.h"

#include "esp_log.h"
#include "esp_heap_caps.h"
#include "driver/i2s_std.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"

#include <stdio.h>
#include <string.h>

static const char *TAG = "sampler";

typedef struct {
    uint32_t start;     // byte offset into audio data
    uint32_t length;    // length in bytes
} slice_t;

typedef struct {
    int16_t *data;          // audio data in PSRAM
    uint32_t data_size;     // total audio data size in bytes
    uint16_t channels;
    uint16_t bits_per_sample;
    uint32_t sample_rate;
    slice_t slices[MAX_SLICES];
    int num_slices;
} kit_t;

typedef struct {
    int active;
    int slice_idx;
    uint32_t pos;       // current byte position within slice
    uint8_t velocity;
} voice_t;

static kit_t s_kit;
static voice_t s_voices[MAX_VOICES];
static i2s_chan_handle_t s_i2s_handle;

// Guards the live kit buffer (s_kit.data / slices) against the render task while
// sample_player_load_kit() swaps in a new kit. Held only for the microsecond swap
// and for each render mix pass — never during the slow SD read.
static SemaphoreHandle_t s_kit_mutex;
static char s_kit_name[64];

static uint32_t read_u32_le(const uint8_t *p)
{
    return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

static uint16_t read_u16_le(const uint8_t *p)
{
    return p[0] | (p[1] << 8);
}

static esp_err_t parse_wav_header(FILE *f, kit_t *k, uint32_t *data_offset, uint32_t *data_size)
{
    uint8_t header[12];
    fread(header, 1, 12, f);

    if (memcmp(header, "RIFF", 4) != 0 || memcmp(header + 8, "WAVE", 4) != 0) {
        ESP_LOGE(TAG, "Not a WAV file");
        return ESP_ERR_INVALID_ARG;
    }

    // Walk chunks
    while (!feof(f)) {
        uint8_t chunk_header[8];
        if (fread(chunk_header, 1, 8, f) != 8) break;

        char chunk_id[5] = {0};
        memcpy(chunk_id, chunk_header, 4);
        uint32_t chunk_size = read_u32_le(chunk_header + 4);

        if (memcmp(chunk_id, "fmt ", 4) == 0) {
            uint8_t fmt[16];
            fread(fmt, 1, 16, f);
            k->channels = read_u16_le(fmt + 2);
            k->sample_rate = read_u32_le(fmt + 4);
            k->bits_per_sample = read_u16_le(fmt + 14);
            ESP_LOGI(TAG, "Format: %d ch, %lu Hz, %d bit",
                     k->channels, k->sample_rate, k->bits_per_sample);
            if (chunk_size > 16) fseek(f, chunk_size - 16, SEEK_CUR);

        } else if (memcmp(chunk_id, "data", 4) == 0) {
            *data_offset = ftell(f);
            *data_size = chunk_size;
            fseek(f, chunk_size, SEEK_CUR);

        } else if (memcmp(chunk_id, "cue ", 4) == 0) {
            uint8_t *cue_data = malloc(chunk_size);
            fread(cue_data, 1, chunk_size, f);

            uint32_t num_cues = read_u32_le(cue_data);
            k->num_slices = (num_cues > MAX_SLICES) ? MAX_SLICES : num_cues;

            uint32_t bytes_per_sample = (k->bits_per_sample / 8) * k->channels;

            for (int i = 0; i < k->num_slices; i++) {
                uint32_t sample_offset = read_u32_le(cue_data + 4 + i * 24 + 20);
                k->slices[i].start = sample_offset * bytes_per_sample;
            }
            free(cue_data);

            ESP_LOGI(TAG, "Found %d cue points", k->num_slices);

        } else {
            fseek(f, chunk_size, SEEK_CUR);
        }

        // Pad byte for odd-sized chunks
        if (chunk_size % 2) fseek(f, 1, SEEK_CUR);
    }

    return ESP_OK;
}

static void compute_slice_lengths(kit_t *k, uint32_t total_data_size)
{
    for (int i = 0; i < k->num_slices; i++) {
        if (i + 1 < k->num_slices) {
            k->slices[i].length = k->slices[i + 1].start - k->slices[i].start;
        } else {
            k->slices[i].length = total_data_size - k->slices[i].start;
        }
        ESP_LOGI(TAG, "Slice %d: offset %lu, length %lu (%.3fs)",
                 i + 1, k->slices[i].start, k->slices[i].length,
                 (float)k->slices[i].length / (k->sample_rate * k->channels * (k->bits_per_sample / 8)));
    }
}

// basename without directory or extension, into s_kit_name.
static void set_kit_name(const char *path)
{
    const char *base = strrchr(path, '/');
    base = base ? base + 1 : path;
    strlcpy(s_kit_name, base, sizeof(s_kit_name));
    char *dot = strrchr(s_kit_name, '.');
    if (dot) *dot = '\0';
}

esp_err_t sample_player_load_kit(const char *path)
{
    ESP_LOGI(TAG, "Loading kit: %s", path);

    // --- Load fully into a local kit; the live kit keeps playing meanwhile ---
    kit_t nk = {0};

    FILE *f = fopen(path, "rb");
    if (!f) {
        ESP_LOGE(TAG, "Failed to open: %s", path);
        return ESP_ERR_NOT_FOUND;
    }

    uint32_t data_offset = 0, data_size = 0;
    esp_err_t ret = parse_wav_header(f, &nk, &data_offset, &data_size);
    if (ret != ESP_OK) {
        fclose(f);
        return ret;
    }

    compute_slice_lengths(&nk, data_size);

    nk.data = heap_caps_malloc(data_size, MALLOC_CAP_SPIRAM);
    if (!nk.data) {
        ESP_LOGE(TAG, "Failed to allocate %lu bytes in PSRAM", (unsigned long)data_size);
        fclose(f);
        return ESP_ERR_NO_MEM;
    }

    fseek(f, data_offset, SEEK_SET);
    nk.data_size = fread(nk.data, 1, data_size, f);
    fclose(f);

    // --- Atomic swap: silence voices, drop the old buffer, install the new ---
    int16_t *old_data = NULL;
    xSemaphoreTake(s_kit_mutex, portMAX_DELAY);
    for (int v = 0; v < MAX_VOICES; v++) s_voices[v].active = 0;
    old_data = s_kit.data;
    s_kit = nk;
    xSemaphoreGive(s_kit_mutex);

    if (old_data) heap_caps_free(old_data); // free the previous kit (fixes reload leak)
    set_kit_name(path);

    ESP_LOGI(TAG, "Kit loaded: %lu bytes in PSRAM (%s)", (unsigned long)nk.data_size, s_kit_name);
    return ESP_OK;
}

const char *sample_player_current_kit(void)
{
    return s_kit_name;
}

void sample_player_note_on(uint8_t note, uint8_t velocity)
{
    int slice_idx = note - BASE_NOTE;

    // Share the kit mutex with the render/swap path so we never read a kit that
    // is mid-swap or index into slices that are being replaced.
    xSemaphoreTake(s_kit_mutex, portMAX_DELAY);

    if (s_kit.data == NULL || slice_idx < 0 || slice_idx >= s_kit.num_slices) {
        xSemaphoreGive(s_kit_mutex);
        return;
    }

    // Find a free voice, or steal the oldest
    int oldest = 0;
    uint32_t oldest_pos = 0;
    for (int i = 0; i < MAX_VOICES; i++) {
        if (!s_voices[i].active) {
            oldest = i;
            break;
        }
        if (s_voices[i].pos > oldest_pos) {
            oldest_pos = s_voices[i].pos;
            oldest = i;
        }
    }

    s_voices[oldest].active = 1;
    s_voices[oldest].slice_idx = slice_idx;
    s_voices[oldest].pos = 0;
    s_voices[oldest].velocity = velocity;

    xSemaphoreGive(s_kit_mutex);

    ESP_LOGD(TAG, "Voice %d -> slice %d (note %d, vel %d)", oldest, slice_idx, note, velocity);
}

void sample_player_note_off(uint8_t note)
{
    // Drums are one-shot — note off is ignored
}

static void i2s_render_task(void *param)
{
    const int buf_samples = 256;
    const int buf_size = buf_samples * 2 * sizeof(int16_t); // stereo
    int16_t *mix_buf = malloc(buf_size);

    ESP_LOGI(TAG, "I2S render task running on core %d", xPortGetCoreID());

    while (1) {
        memset(mix_buf, 0, buf_size);

        // Non-blocking: if a kit swap is in progress, emit this one buffer silent
        // rather than stall the audio pipeline (swap holds the mutex only briefly).
        if (xSemaphoreTake(s_kit_mutex, 0) == pdTRUE) {
            if (s_kit.data) {
                for (int v = 0; v < MAX_VOICES; v++) {
                    if (!s_voices[v].active) continue;

                    slice_t *slice = &s_kit.slices[s_voices[v].slice_idx];
                    uint32_t pos = s_voices[v].pos;
                    uint32_t remaining = slice->length - pos;
                    uint32_t to_copy = (remaining < (uint32_t)buf_size) ? remaining : (uint32_t)buf_size;
                    uint32_t num_samples = to_copy / sizeof(int16_t);

                    int16_t *src = (int16_t *)((uint8_t *)s_kit.data + slice->start + pos);
                    int vel_scale = s_voices[v].velocity;

                    for (uint32_t i = 0; i < num_samples; i++) {
                        int32_t sample = (int32_t)src[i] * vel_scale / 127;
                        int32_t mixed = (int32_t)mix_buf[i] + sample;
                        // Clamp
                        if (mixed > 32767) mixed = 32767;
                        if (mixed < -32768) mixed = -32768;
                        mix_buf[i] = (int16_t)mixed;
                    }

                    s_voices[v].pos += to_copy;
                    if (s_voices[v].pos >= slice->length) {
                        s_voices[v].active = 0;
                    }
                }
            }
            xSemaphoreGive(s_kit_mutex);
        }

        size_t bytes_written;
        i2s_channel_write(s_i2s_handle, mix_buf, buf_size, &bytes_written, portMAX_DELAY);
    }
}

esp_err_t sample_player_init(void)
{
    i2s_chan_config_t chan_cfg = I2S_CHANNEL_DEFAULT_CONFIG(I2S_NUM_0, I2S_ROLE_MASTER);
    ESP_ERROR_CHECK(i2s_new_channel(&chan_cfg, &s_i2s_handle, NULL));

    i2s_std_config_t std_cfg = {
        .clk_cfg = I2S_STD_CLK_DEFAULT_CONFIG(SAMPLE_RATE),
        .slot_cfg = I2S_STD_PHILIPS_SLOT_DEFAULT_CONFIG(I2S_DATA_BIT_WIDTH_16BIT, I2S_SLOT_MODE_STEREO),
        .gpio_cfg = {
            .mclk = I2S_MCLK,
            .bclk = I2S_BCLK,
            .ws = I2S_WS,
            .dout = I2S_DOUT,
            .din = I2S_GPIO_UNUSED,
            .invert_flags = {
                .mclk_inv = false,
                .bclk_inv = false,
                .ws_inv = false,
            },
        },
    };
    ESP_ERROR_CHECK(i2s_channel_init_std_mode(s_i2s_handle, &std_cfg));
    ESP_ERROR_CHECK(i2s_channel_enable(s_i2s_handle));

    memset(s_voices, 0, sizeof(s_voices));
    s_kit_mutex = xSemaphoreCreateMutex();

    xTaskCreatePinnedToCore(i2s_render_task, "i2s_render", 4096, NULL, 6, NULL, SYNTH_CORE);

    ESP_LOGI(TAG, "Sample player initialized (I2S on core %d)", SYNTH_CORE);
    return ESP_OK;
}
