/**
 * GPIO Tester for Seeed XIAO ESP32S3
 * Loops through all 11 digital IO pins, drives them HIGH for 250ms,
 * and logs state before/after each trigger.
 *
 * Pin Mapping (XIAO label -> GPIO):
 *   D0  -> GPIO1   D1  -> GPIO2   D2  -> GPIO3
 *   D3  -> GPIO4   D4  -> GPIO5   D5  -> GPIO6
 *   D6  -> GPIO43  D7  -> GPIO44  D8  -> GPIO7
 *   D9  -> GPIO8   D10 -> GPIO9
 *
 * NOTE: D6 (GPIO43) = TX and D7 (GPIO44) = RX of the hardware UART used
 *       for Serial. Triggering them will corrupt Serial output momentarily.
 *       They are included but clearly flagged in the log.
 */

// ── Pin table ──────────────────────────────────────────────────────────────
struct PinEntry {
  uint8_t     arduinoPin;   // Arduino/XIAO symbolic constant (D0 … D10)
  uint8_t     gpioNum;      // Underlying ESP32-S3 GPIO number
  const char* label;        // Human-readable label
  bool        isUartPin;    // Flag pins shared with Serial
};

static const PinEntry PINS[] = {
  { D0,  1,  "D0  (GPIO1 / A0)",          false },
  { D1,  2,  "D1  (GPIO2 / A1)",          false },
  { D2,  3,  "D2  (GPIO3 / A2)",          false },
  { D3,  4,  "D3  (GPIO4 / A3)",          false },
  { D4,  5,  "D4  (GPIO5 / SDA)",         false },
  { D5,  6,  "D5  (GPIO6 / SCL)",         false },
  { D6,  43, "D6  (GPIO43 / TX) [UART!]", true  },
  { D7,  44, "D7  (GPIO44 / RX) [UART!]", true  },
  { D8,  7,  "D8  (GPIO7  / SCK)",        false },
  { D9,  8,  "D9  (GPIO8  / MISO)",       false },
  { D10, 9,  "D10 (GPIO9  / MOSI)",       false },
};

static const uint8_t PIN_COUNT    = sizeof(PINS) / sizeof(PINS[0]);
static const uint16_t TRIGGER_MS  = 1000;   // HIGH duration
static const uint16_t SETTLE_MS   = 500;   // LOW settle time between pins
static const uint32_t BAUD        = 115200;

// ── Helpers ────────────────────────────────────────────────────────────────
void separator() {
  Serial.println(F("──────────────────────────────────────────────"));
}

void printState(const PinEntry& p, int state, const char* moment) {
  Serial.print(F("  ["));
  Serial.print(moment);
  Serial.print(F("] "));
  Serial.print(p.label);
  Serial.print(F(" -> "));
  Serial.println(state == HIGH ? F("HIGH") : F("LOW"));
}

// ── Setup ──────────────────────────────────────────────────────────────────
void setup() {
  Serial.begin(BAUD);
  delay(2000);   // wait for Serial monitor to connect

  Serial.println();
  separator();
  Serial.println(F("  XIAO ESP32S3 — GPIO Pin Tester"));
  Serial.print  (F("  Testing "));
  Serial.print  (PIN_COUNT);
  Serial.println(F(" pins | HIGH for 250 ms each"));
  separator();
  Serial.println();

  // Configure all pins as OUTPUT, start LOW
  for (uint8_t i = 0; i < PIN_COUNT; i++) {
    pinMode(PINS[i].arduinoPin, OUTPUT);
    digitalWrite(PINS[i].arduinoPin, LOW);
  }
}

// ── Main loop ──────────────────────────────────────────────────────────────
void loop() {
  static uint16_t round = 0;
  round++;

  Serial.print(F("\n=== Test Round #"));
  Serial.print(round);
  Serial.println(F(" ===\n"));

  for (uint8_t i = 0; i < PIN_COUNT; i++) {
    const PinEntry& p = PINS[i];

    separator();
    Serial.print(F("Pin ["));
    Serial.print(i + 1);
    Serial.print(F("/"));
    Serial.print(PIN_COUNT);
    Serial.print(F("] "));
    Serial.println(p.label);
    if (p.isUartPin) {
      Serial.println(F("  *** Shared with Serial UART — output may glitch briefly ***"));
    }

    // PRE-trigger log
    printState(p, digitalRead(p.arduinoPin), "BEFORE");

    // Trigger HIGH
    digitalWrite(p.arduinoPin, HIGH);

    // Flush Serial before touching TX/RX pins
    if (p.isUartPin) Serial.flush();

    delay(TRIGGER_MS);

    // POST-trigger log (re-open Serial if UART pin)
    if (p.isUartPin) {
      Serial.begin(BAUD);   // re-sync after pin was pulled
      delay(10);
    }

    printState(p, digitalRead(p.arduinoPin), "AFTER ");

    // Return to LOW, settle
    digitalWrite(p.arduinoPin, LOW);
    delay(SETTLE_MS);
  }

  separator();
  Serial.println(F("\nAll pins tested. Waiting 3 s before next round…\n"));
  delay(3000);
}