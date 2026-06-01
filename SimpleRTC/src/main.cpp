#include <Arduino.h>
#include <Wire.h>
#include "DS1307.h"

DS1307 rtc;

// ── I2C scan ──────────────────────────────────────────────────────────────────
void scanI2C() {
    Serial.println("Scanning I2C bus...");
    uint8_t found = 0;
    for (uint8_t addr = 1; addr < 127; addr++) {
        Wire.beginTransmission(addr);
        if (Wire.endTransmission() == 0) {
            Serial.printf("  Device found at 0x%02X\n", addr);
            found++;
        }
    }
    if (found == 0) Serial.println("  No I2C devices found.");
    Serial.println();
}

// ── Unix timestamp from DS1307 fields ─────────────────────────────────────────
// rtc.getTime() must be called first.
// rtc.year is 2-digit (0-99 representing 2000-2099).
uint32_t computeUnixTime() {
    uint16_t y = (uint16_t)rtc.year + 2000;
    uint8_t  m = rtc.month;
    uint8_t  d = rtc.dayOfMonth;
    if (m < 3) { m += 12; y--; }
    uint32_t days = 365UL * y + y/4 - y/100 + y/400
                  + (153 * m - 457) / 5 + d - 719469UL;
    return days * 86400UL
         + (uint32_t)rtc.hour   * 3600UL
         + (uint32_t)rtc.minute * 60UL
         + rtc.second;
}

// ── Validate RTC fields are in-range (detects 0xFF garbage on I2C failure) ────
bool rtcDataValid() {
    return rtc.month  >= 1 && rtc.month  <= 12 &&
           rtc.hour   <= 23 &&
           rtc.minute <= 59 &&
           rtc.second <= 59;
}

void setup() {
    Serial.begin(9600);
    delay(3000);
    Serial.println("\n=== SimpleRTC — DS1307 test ===");

    Wire.begin();  // XIAO ESP32-S3: SDA=GPIO5 (D4), SCL=GPIO6 (D5)
    Wire.setTimeOut(1000);

    scanI2C();

    // Check DS1307 is present on I2C before talking to it.
    Wire.beginTransmission(0x68);
    if (Wire.endTransmission() != 0) {
        Serial.println("ERROR: DS1307 not found at 0x68. Check SDA/SCL/VCC/GND wiring.");
        return;
    }
    Serial.println("DS1307 found at 0x68.");

    rtc.begin();

    // ── Set the time once ──────────────────────────────────────────────────────
    // Uncomment these three lines, flash, then re-comment and reflash.
     rtc.fillByYMD(2026, 5, 31);
     rtc.fillByHMS(23, 10, 0);
     rtc.setTime();
    // ──────────────────────────────────────────────────────────────────────────

    Serial.println("Setup complete. Reading time every second...\n");
}

void loop() {
    Wire.beginTransmission(0x68);
    if (Wire.endTransmission() != 0) {
        Serial.println("DS1307 not responding.");
        delay(1000);
        return;
    }

    rtc.getTime();

    if (!rtcDataValid()) {
        Serial.println("RTC returned invalid data (0xFF) — clock may not be set or wiring issue.");
        delay(1000);
        return;
    }

    uint32_t unixTs = computeUnixTime();

    Serial.printf("Time:      %02d:%02d:%02d\n", rtc.hour, rtc.minute, rtc.second);
    Serial.printf("Date:      %04d-%02d-%02d\n", rtc.year + 2000, rtc.month, rtc.dayOfMonth);
    Serial.printf("Unix time: %lu\n\n", (unsigned long)unixTs);

    delay(1000);
}
