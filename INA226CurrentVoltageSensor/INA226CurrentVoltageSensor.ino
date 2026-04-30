#include <Wire.h>
#include <INA226_WE.h>

INA226_WE ina226 = INA226_WE(0x40);  // Default I2C addr

void setup() {
  Serial.begin(9600);
  Wire.begin();
  if (!ina226.init()) {
    Serial.println("INA226 not found!");
    while (1);
  }
  ina226.setAverage(INA226_AVERAGE_16);  // Smooth
  ina226.setConversionTime(INA226_CONV_TIME_1100);  // µs
  ina226.setMeasureMode(INA226_CONTINUOUS);  // Or TRIGGERED
  ina226.waitUntilConversionCompleted();  // Initial
}

void loop() {
  float vBus = ina226.getBusVoltage_V();  // V
  float current_mA = ina226.getCurrent_mA();  // Convert to A: /1000.0
  Serial.print("Voltage: "); Serial.print(vBus); Serial.println(" V");
  Serial.print("Current: "); Serial.print(current_mA / 1000.0); Serial.println(" A");
  delay(1000);
}