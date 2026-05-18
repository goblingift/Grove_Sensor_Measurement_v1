#include <Wire.h>

#ifdef ARDUINO_SAMD_VARIANT_COMPLIANCE
#define SERIAL_PORT SerialUSB
#else
#define SERIAL_PORT Serial
#endif

#define THRESHOLD 100
#define HIGH_ADDR 0x78
#define LOW_ADDR  0x77

byte lowData[8], highData[12];

void readBytes(byte addr, byte *buf, byte len) {
  Wire.requestFrom(addr, len);
  while (Wire.available() < len);
  for (byte i = 0; i < len; i++) buf[i] = Wire.read();
}

int readWaterLevelPercent() {
  uint32_t mask = 0;

  readBytes(LOW_ADDR, lowData, 8);
  readBytes(HIGH_ADDR, highData, 12);

  for (byte i = 0; i < 8; i++) {
    if (lowData[i] > THRESHOLD) mask |= (1UL << i);
  }

  for (byte i = 0; i < 12; i++) {
    if (highData[i] > THRESHOLD) mask |= (1UL << (8 + i));
  }

  byte sections = 0;
  while (mask & 1) {
    sections++;
    mask >>= 1;
  }

  return sections * 5;
}

void setup() {
  SERIAL_PORT.begin(115200);
  Wire.begin();
}

void loop() {
  SERIAL_PORT.print(readWaterLevelPercent());
  SERIAL_PORT.println("%");
  delay(10000);
}