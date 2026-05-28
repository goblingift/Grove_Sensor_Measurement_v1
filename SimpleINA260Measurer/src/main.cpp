#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_INA260.h>

Adafruit_INA260 ina260;

void setup() {
  Serial.begin(115200);
  while (!Serial) delay(10);

  Wire.begin();

  if (!ina260.begin()) {
    Serial.println("Could not find INA260 chip. Check wiring!");
    while (1) delay(100);
  }

  Serial.println("INA260 found.");
}

void loop() {
  float current_mA = ina260.readCurrent();
  float voltage_mV = ina260.readBusVoltage();
  float power_mW   = ina260.readPower();

  Serial.print("Current: "); Serial.print(current_mA); Serial.println(" mA");
  Serial.print("Voltage: "); Serial.print(voltage_mV / 1000.0); Serial.println(" V");
  Serial.print("Power:   "); Serial.print(power_mW / 1000.0); Serial.println(" W");
  Serial.println("---");

  delay(1000);
}
