// ============================================================
// XIAO MIDI Synthesizer — BLE MIDI Bridge + Voice Navigator
// RYNGO — Drums (Channel 10)
// ESP32-C3 + SAM2695
//
// Libraries required:
//   - esp32 by Espressif v2.0.17 (Boards Manager)
//   - NimBLE-Arduino by h2zero v1.4.2 (Library Manager)
//   - BLE-MIDI by lathoub (Library Manager)
//   - MIDI Library by lathoub (Library Manager)
//   - Seeed_Arduino_MIDIMaster (manual ZIP install)
//     https://github.com/Seeed-Studio/Seeed_Arduino_MIDIMaster
//
// Board: XIAO ESP32-C3
// In Arduino IDE: Tools > Board > esp32 > XIAO_ESP32C3
//
// ============================================================
// RYNGO CONFIG — Drums on Channel 10
#define DEVICE_NAME    "Ryngo"
#define MIDI_CHANNEL   10           // GM drum channel
#define DEFAULT_PATCH  0            // 0=Standard Kit
#define DEFAULT_BANK   0            // 0=General MIDI
// ============================================================
//
// DRUM KITS (Program Change on Channel 10):
//   0  = Standard Kit
//   8  = Room Kit
//   16 = Power Kit
//   24 = Electronic Kit
//   25 = TR-808 Kit
//   32 = Jazz Kit
//   40 = Brush Kit
//   48 = Orchestra Kit
//   56 = SFX Kit
//
// BUTTON MAPPING:
//   Button 0: Next drum kit        | Long: (reserved)
//   Button 1: Previous drum kit    | Long: Volume up (+10)
//   Button 2: Next kit (+1)        | Long: Volume down (-10)
//   Button 3: Previous kit (-1)    | Long: MIDI panic (all notes off)
//
// SAM2695 CC REFERENCE (send from AUM / TouchOSC):
//   CC 7  - Channel Volume
//   CC 10 - Pan             (0=left, 64=center, 127=right)
//   CC 11 - Expression
//   CC 91 - Reverb Send     (0-127)
//   CC 93 - Chorus Send     (0-127)
//   CC 120 - All Sound Off
//   CC 123 - All Notes Off
// ============================================================

#include <Arduino.h>

// --- BLE MIDI via lathoub + NimBLE ---
#include <BLEMIDI_Transport.h>
#include <hardware/BLEMIDI_ESP32_NimBLE.h>
BLEMIDI_CREATE_INSTANCE(DEVICE_NAME, MIDI)

#include "SAM2695Synth.h"

// ----------------------------------------------------------
// Hardware pins for XIAO ESP32-C3 MIDI Synthesizer board
// ----------------------------------------------------------
#define BUTTON_0_PIN  5
#define BUTTON_1_PIN  4
#define BUTTON_2_PIN  3
#define BUTTON_3_PIN  2
#define LED_BUILTIN   10

// ----------------------------------------------------------
// Button timing
// ----------------------------------------------------------
#define DEBOUNCE_MS       50
#define LONG_PRESS_MS     600

// ----------------------------------------------------------
// Reverb type names (CC 80, values 0-7)
// ----------------------------------------------------------
const char* REVERB_TYPES[] = {
  "Room 1", "Room 2", "Room 3",
  "Hall 1", "Hall 2",
  "Plate", "Delay", "Pan Delay"
};

// ----------------------------------------------------------
// Chorus type names (CC 81, values 0-7)
// ----------------------------------------------------------
const char* CHORUS_TYPES[] = {
  "Chorus 1", "Chorus 2", "Chorus 3", "Chorus 4",
  "FB Chorus", "Flanger",
  "Short Delay", "FB Delay"
};

// ----------------------------------------------------------
// Drum kit names (for channel 10 program changes)
// ----------------------------------------------------------
const uint8_t DRUM_KIT_PROGRAMS[] = {0, 8, 16, 24, 25, 32, 40, 48, 56};
const char* DRUM_KIT_NAMES[] = {
  "Standard Kit",
  "Room Kit",
  "Power Kit",
  "Electronic Kit",
  "TR-808 Kit",
  "Jazz Kit",
  "Brush Kit",
  "Orchestra Kit",
  "SFX Kit"
};
const uint8_t NUM_DRUM_KITS = 9;

// ----------------------------------------------------------
// Voice state
// ----------------------------------------------------------
const uint8_t LOCAL_CH  = MIDI_CHANNEL - 1;  // 0-indexed channel for SAM2695 (9 for drums)
uint8_t currentProgram  = DEFAULT_PATCH;
uint8_t currentBank     = DEFAULT_BANK;
uint8_t currentKitIndex = 0;  // Index into DRUM_KIT_PROGRAMS
uint8_t currentVolume   = 100;

// ----------------------------------------------------------
// SAM2695 synth instance
// ----------------------------------------------------------
SAM2695Synth<HardwareSerial> synth = SAM2695Synth<HardwareSerial>::getInstance();

// ----------------------------------------------------------
// BLE connection state
// ----------------------------------------------------------
bool bleConnected = false;

// ----------------------------------------------------------
// Raw MIDI helpers for CCs the Seeed library doesn't expose
// ----------------------------------------------------------
void sendCC(uint8_t channel, uint8_t cc, uint8_t value) {
  Serial0.write(0xB0 | (channel & 0x0F));
  Serial0.write(cc & 0x7F);
  Serial0.write(value & 0x7F);
}

void sendBankAndProgram(uint8_t channel, uint8_t bank, uint8_t program) {
  sendCC(channel, 0x00, bank);   // Bank Select MSB
  Serial0.write(0xC0 | (channel & 0x0F));
  Serial0.write(program & 0x7F);
}

// ----------------------------------------------------------
// Find kit index from program number
// ----------------------------------------------------------
uint8_t findKitIndex(uint8_t program) {
  for (uint8_t i = 0; i < NUM_DRUM_KITS; i++) {
    if (DRUM_KIT_PROGRAMS[i] == program) return i;
  }
  return 0; // default to Standard Kit
}

// ----------------------------------------------------------
// Apply current drum kit and print status
// ----------------------------------------------------------
void applyKit() {
  currentProgram = DRUM_KIT_PROGRAMS[currentKitIndex];
  sendBankAndProgram(LOCAL_CH, currentBank, currentProgram);
  Serial.print("Drum Kit: [");
  Serial.print(currentProgram);
  Serial.print("] ");
  Serial.println(DRUM_KIT_NAMES[currentKitIndex]);
}

// ----------------------------------------------------------
// Button state tracking
// ----------------------------------------------------------
struct Button {
  uint8_t  pin;
  bool     lastState;
  bool     currentState;
  uint32_t pressTime;
  bool     longFired;
};

Button buttons[4] = {
  {BUTTON_0_PIN, HIGH, HIGH, 0, false},
  {BUTTON_1_PIN, HIGH, HIGH, 0, false},
  {BUTTON_2_PIN, HIGH, HIGH, 0, false},
  {BUTTON_3_PIN, HIGH, HIGH, 0, false},
};

void onShortPress(uint8_t idx) {
  switch (idx) {
    case 0: // Next drum kit (jump)
      currentKitIndex = (currentKitIndex + 1) % NUM_DRUM_KITS;
      applyKit();
      break;
    case 1: // Previous drum kit (jump)
      currentKitIndex = (currentKitIndex == 0) ? NUM_DRUM_KITS - 1 : currentKitIndex - 1;
      applyKit();
      break;
    case 2: // Next kit (+1 fine)
      currentKitIndex = (currentKitIndex + 1) % NUM_DRUM_KITS;
      applyKit();
      break;
    case 3: // Previous kit (-1 fine)
      currentKitIndex = (currentKitIndex == 0) ? NUM_DRUM_KITS - 1 : currentKitIndex - 1;
      applyKit();
      break;
  }
}

void onLongPress(uint8_t idx) {
  switch (idx) {
    case 0: // Reserved (no MT-32 drums)
      Serial.println("(No MT-32 drum kits)");
      break;
    case 1: // Volume up
      currentVolume = min(127, (int)currentVolume + 10);
      synth.setVolume(LOCAL_CH, currentVolume);
      Serial.print("Volume: ");
      Serial.println(currentVolume);
      break;
    case 2: // Volume down
      currentVolume = max(0, (int)currentVolume - 10);
      synth.setVolume(LOCAL_CH, currentVolume);
      Serial.print("Volume: ");
      Serial.println(currentVolume);
      break;
    case 3: // MIDI panic — all notes off
      for (uint8_t ch = 0; ch < 16; ch++) {
        synth.setAllNotesOff(ch);
      }
      Serial.println("PANIC: All notes off (all channels)");
      break;
  }
}

void pollButtons() {
  uint32_t now = millis();
  for (uint8_t i = 0; i < 4; i++) {
    bool reading = digitalRead(buttons[i].pin);
    if (reading == LOW && buttons[i].currentState == HIGH) {
      buttons[i].currentState = LOW;
      buttons[i].pressTime    = now;
      buttons[i].longFired    = false;
    }
    else if (reading == HIGH && buttons[i].currentState == LOW) {
      buttons[i].currentState = HIGH;
      if (!buttons[i].longFired && (now - buttons[i].pressTime < LONG_PRESS_MS)) {
        onShortPress(i);
      }
    }
    else if (reading == LOW && buttons[i].currentState == LOW) {
      if (!buttons[i].longFired && (now - buttons[i].pressTime >= LONG_PRESS_MS)) {
        buttons[i].longFired = true;
        onLongPress(i);
      }
    }
    buttons[i].lastState = reading;
  }
}

// ----------------------------------------------------------
// BLE MIDI callbacks — lathoub style
// ----------------------------------------------------------
void handleNoteOn(byte channel, byte note, byte velocity) {
  // Always send to drum channel (LOCAL_CH = 9)
  if (velocity == 0) {
    synth.setNoteOff(LOCAL_CH, note);
  } else {
    synth.setNoteOn(LOCAL_CH, note, velocity);
  }
}

void handleNoteOff(byte channel, byte note, byte velocity) {
  // Always send to drum channel (LOCAL_CH = 9)
  synth.setNoteOff(LOCAL_CH, note);
}

void handleProgramChange(byte channel, byte program) {
  sendBankAndProgram(channel - 1, currentBank, program);
  if (channel == MIDI_CHANNEL) {
    currentProgram  = program;
    currentKitIndex = findKitIndex(program);
    Serial.print("BLE PC -> [");
    Serial.print(program);
    Serial.print("] ");
    Serial.println(DRUM_KIT_NAMES[currentKitIndex]);
  }
}

void handleControlChange(byte channel, byte cc, byte value) {
  uint8_t ch = channel - 1; // convert to 0-indexed for SAM2695

  switch (cc) {
    // --- Volume via Seeed library ---
    case 7:
      synth.setVolume(ch, value);
      if (ch == LOCAL_CH) currentVolume = value;
      Serial.print("Vol ch"); Serial.print(channel);
      Serial.print(": "); Serial.println(value);
      break;

    // --- Bank Select ---
    case 0:
      if (ch == LOCAL_CH) currentBank = value;
      sendCC(ch, 0x00, value);
      break;

    // --- Modulation ---
    case 1:
      sendCC(ch, 0x01, value);
      break;

    // --- Pan ---
    case 10:
      sendCC(ch, 0x0A, value);
      Serial.print("Pan ch"); Serial.print(channel);
      Serial.print(": "); Serial.println(value);
      break;

    // --- Expression ---
    case 11:
      sendCC(ch, 0x0B, value);
      break;

    // --- Reverb Type (0-7) ---
    case 80:
      sendCC(ch, 0x50, value & 0x07);
      Serial.print("Reverb type: ");
      Serial.println(REVERB_TYPES[value & 0x07]);
      break;

    // --- Chorus Type (0-7) ---
    case 81:
      sendCC(ch, 0x51, value & 0x07);
      Serial.print("Chorus type: ");
      Serial.println(CHORUS_TYPES[value & 0x07]);
      break;

    // --- Reverb Send ---
    case 91:
      sendCC(ch, 0x5B, value);
      Serial.print("Reverb send: "); Serial.println(value);
      break;

    // --- Chorus Send ---
    case 93:
      sendCC(ch, 0x5D, value);
      Serial.print("Chorus send: "); Serial.println(value);
      break;

    // --- All Sound Off / All Notes Off ---
    case 120:
    case 123:
      synth.setAllNotesOff(ch);
      break;

    // --- Reset All Controllers ---
    case 121:
      sendCC(ch, 0x79, 0);
      break;

    // --- Pass anything else through raw ---
    default:
      sendCC(ch, cc, value);
      break;
  }
}

void handlePitchBend(byte channel, int bend) {
  // Pitch bend on drums — some kits respond to this
  uint16_t pb  = (uint16_t)(bend + 8192);
  uint8_t  lsb = pb & 0x7F;
  uint8_t  msb = (pb >> 7) & 0x7F;
  Serial0.write(0xE0 | ((channel - 1) & 0x0F));
  Serial0.write(lsb);
  Serial0.write(msb);
}

void handleAfterTouch(byte channel, byte pressure) {
  // Channel aftertouch
  Serial0.write(0xD0 | ((channel - 1) & 0x0F));
  Serial0.write(pressure & 0x7F);
}

// ----------------------------------------------------------
// Setup
// ----------------------------------------------------------
void setup() {
  Serial.begin(115200);
  delay(1000);

  Serial.println("=================================");
  Serial.print("  "); Serial.println(DEVICE_NAME);
  Serial.println("  BLE MIDI Bridge — Drums");
  Serial.println("=================================");

  // Button pins — active LOW, internal pullup
  for (uint8_t i = 0; i < 4; i++) {
    pinMode(buttons[i].pin, INPUT_PULLUP);
  }

  // LED
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH); // off (active LOW)

  // Initialize SAM2695
  synth.begin(Serial0, 31250);
  delay(600); // SAM2695 needs ~500ms after power-up

  // Init all 16 channels
  for (uint8_t ch = 0; ch < 16; ch++) {
    synth.setVolume(ch, 100);
    delay(10);
  }

  // Set default drum kit
  sendBankAndProgram(LOCAL_CH, currentBank, currentProgram);
  delay(50);

  // Make sure drum channel volume is up
  synth.setVolume(LOCAL_CH, 127);  // Max volume for drums
  currentVolume = 127;

  // Default reverb/chorus settings for drums
  sendCC(LOCAL_CH, 0x50, 1);   // Reverb type: Room 2
  sendCC(LOCAL_CH, 0x5B, 30);  // Reverb send: 30
  sendCC(LOCAL_CH, 0x51, 0);   // Chorus type: Chorus 1
  sendCC(LOCAL_CH, 0x5D, 0);   // Chorus send: 0

  applyKit();

  // Boot sound — signature beat
  Serial.println("Ryngo says hello...");
  // Kick
  synth.setNoteOn(LOCAL_CH, 36, 127);
  delay(150);
  synth.setNoteOff(LOCAL_CH, 36);
  // Hi-hat
  synth.setNoteOn(LOCAL_CH, 42, 90);
  delay(150);
  synth.setNoteOff(LOCAL_CH, 42);
  // Snare
  synth.setNoteOn(LOCAL_CH, 38, 127);
  delay(150);
  synth.setNoteOff(LOCAL_CH, 38);
  // Hi-hat
  synth.setNoteOn(LOCAL_CH, 42, 90);
  delay(150);
  synth.setNoteOff(LOCAL_CH, 42);
  // Kick kick
  synth.setNoteOn(LOCAL_CH, 36, 127);
  delay(100);
  synth.setNoteOff(LOCAL_CH, 36);
  synth.setNoteOn(LOCAL_CH, 36, 100);
  delay(150);
  synth.setNoteOff(LOCAL_CH, 36);
  // Crash
  synth.setNoteOn(LOCAL_CH, 49, 100);
  delay(400);
  synth.setNoteOff(LOCAL_CH, 49);

  // --- BLE MIDI setup (lathoub / NimBLE) ---
  // Listen only to channel 10 (drums)
  MIDI.begin(MIDI_CHANNEL);
  MIDI.setHandleNoteOn(handleNoteOn);
  MIDI.setHandleNoteOff(handleNoteOff);
  MIDI.setHandleProgramChange(handleProgramChange);
  MIDI.setHandleControlChange(handleControlChange);
  MIDI.setHandlePitchBend(handlePitchBend);
  MIDI.setHandleAfterTouchChannel(handleAfterTouch);

  // Connection callbacks
  BLEMIDI.setHandleConnected([]() {
    bleConnected = true;
    digitalWrite(LED_BUILTIN, LOW); // solid on
    Serial.println("BLE connected!");
  });
  BLEMIDI.setHandleDisconnected([]() {
    bleConnected = false;
    digitalWrite(LED_BUILTIN, HIGH);
    Serial.println("BLE disconnected — advertising...");
  });

  Serial.print("BLE advertising as '");
  Serial.print(DEVICE_NAME);
  Serial.print("' on MIDI channel ");
  Serial.println(MIDI_CHANNEL);
  Serial.println();
  Serial.println("Buttons:");
  Serial.println("  [0] Next kit           [0 long] (reserved)");
  Serial.println("  [1] Prev kit           [1 long] Volume up");
  Serial.println("  [2] Next kit (+1)      [2 long] Volume down");
  Serial.println("  [3] Prev kit (-1)      [3 long] MIDI panic");
  Serial.println();
  Serial.println("Drum Kits: Standard, Room, Power, Electronic,");
  Serial.println("           TR-808, Jazz, Brush, Orchestra, SFX");
}

// ----------------------------------------------------------
// Loop
// ----------------------------------------------------------
void loop() {
  // MUST call MIDI.read() every loop for lathoub library
  MIDI.read();

  pollButtons();

  // LED: solid = connected, slow blink = waiting
  if (!bleConnected) {
    uint32_t t = millis() % 1000;
    digitalWrite(LED_BUILTIN, t < 100 ? LOW : HIGH);
  }
}
