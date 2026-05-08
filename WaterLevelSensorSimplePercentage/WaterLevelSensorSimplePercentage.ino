#include <Wire.h>

#define THRESHOLD 100
#define ATTINY1_HIGH_ADDR 0x78
#define ATTINY2_LOW_ADDR  0x77

uint8_t low_data[8];
uint8_t high_data[12];

void readLow() {
  Wire.requestFrom(ATTINY2_LOW_ADDR, 8);
  for (int i = 0; i < 8; i++) {
    low_data[i] = Wire.read();
  }
}

void readHigh() {
  Wire.requestFrom(ATTINY1_HIGH_ADDR, 12);
  for (int i = 0; i < 12; i++) {
    high_data[i] = Wire.read();
  }
}

int getWaterLevel() {
  uint32_t touch_val = 0;

  readLow();
  readHigh();

  for (int i = 0; i < 8; i++) {
    if (low_data[i] > THRESHOLD) touch_val |= (1UL << i);
  }

  for (int i = 0; i < 12; i++) {
    if (high_data[i] > THRESHOLD) touch_val |= (1UL << (8 + i));
  }

  int trig_section = 0;
  while (touch_val & 0x01) {
    trig_section++;
    touch_val >>= 1;
  }

  return trig_section * 5;
}

void setup() {
  Serial.begin(115200);
  Wire.begin();
}

void loop() {
  int level = getWaterLevel();
  Serial.print("Water level: ");
  Serial.print(level);
  Serial.println("%");
  delay(1000);
}