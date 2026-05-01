#include <Wire.h>

void setup() {
  Serial.begin(9600);
  while (!Serial);  // Wait for serial
  Serial.println("\n=== I2C Scanner ===");
  Wire.begin();
  Serial.println("Scanning addresses 0x01 to 0x7F...");
}

void loop() {
  byte error, address;
  int devices = 0;

  Serial.println("Scanning...");
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    error = Wire.endTransmission();

    if (error == 0) {
      Serial.print("Device found at 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println(" !");
      devices++;
    } else if (error == 4) {
      Serial.print("Unknown error at 0x");
      if (address < 16) Serial.print("0");
      Serial.print(address, HEX);
      Serial.println();
    }
  }

  if (devices == 0) {
    Serial.println("No I2C devices found\n");
  } else {
    Serial.print("Found ");
    Serial.print(devices);
    Serial.println(" device(s)\n");
  }

  delay(5000);  // Scan every 5 seconds
}