#include <Wire.h>

#define WTR_LVL_THRESHOLD 100
#define WTR_LVL_HIGH_ADDR 0x78
#define WTR_LVL_LOW_ADDR  0x77

byte WTR_LVL_lowData[8];
byte WTR_LVL_highData[12];

void WTR_LVL_readBytes(byte addr, byte *buf, byte len) {
  Wire.requestFrom(addr, len);
  while (Wire.available() < len);
  for (byte i = 0; i < len; i++) buf[i] = Wire.read();
}

int WTR_LVL_readPercent() {
  uint32_t WTR_LVL_mask = 0;

  WTR_LVL_readBytes(WTR_LVL_LOW_ADDR, WTR_LVL_lowData, 8);
  WTR_LVL_readBytes(WTR_LVL_HIGH_ADDR, WTR_LVL_highData, 12);

  for (byte i = 0; i < 8; i++) {
    if (WTR_LVL_lowData[i] > WTR_LVL_THRESHOLD) WTR_LVL_mask |= (1UL << i);
  }

  for (byte i = 0; i < 12; i++) {
    if (WTR_LVL_highData[i] > WTR_LVL_THRESHOLD) WTR_LVL_mask |= (1UL << (8 + i));
  }

  byte WTR_LVL_sections = 0;
  while (WTR_LVL_mask & 1) {
    WTR_LVL_sections++;
    WTR_LVL_mask >>= 1;
  }

  return WTR_LVL_sections * 5;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
}

void loop() {
  Serial.print(WTR_LVL_readPercent());
  Serial.println("%");
  delay(10000);
}