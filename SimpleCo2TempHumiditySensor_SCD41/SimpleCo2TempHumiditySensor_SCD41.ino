#include <Arduino.h>
#include <SensirionI2cScd4x.h>
#include <Wire.h>

SensirionI2cScd4x sensor;
static int16_t error;
static char errorMessage[64];

#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

void setup() {
  Serial.begin(115200);
  initializeSCD41();
  Serial.println("CO2, Temp, Humidity Sensor starts work!");

}

void loop() {

    uint16_t co2 = 0;
    float temp = 0.0;
    float rh = 0.0;

    error = sensor.wakeUp();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute wakeUp(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }

    // Trigger first measurement after wake up, but ignore values
    error = sensor.measureSingleShot();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute measureSingleShot(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }

    error = sensor.measureAndReadSingleShot(co2, temp, rh);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute measureAndReadSingleShot(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    } else {
      printMeasurement(co2, temp, rh);
    }
  delay(5000);
}

void printMeasurement(uint16_t co2, float temp, float rh) {
    Serial.print("CO2 concentration [ppm]: ");
    Serial.print(co2);
    Serial.println();
    Serial.print("Temperature [°C]: ");
    Serial.print(temp);
    Serial.println();
    Serial.print("Relative Humidity [RH]: ");
    Serial.print(rh);
    Serial.println();
}

void initializeSCD41() {
  Wire.begin();
  sensor.begin(Wire, SCD41_I2C_ADDR_62);
  uint64_t serialNumber = 0;
  delay(30);

  error = sensor.wakeUp();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute wakeUp(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
    }
    error = sensor.stopPeriodicMeasurement();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
    }
    error = sensor.reinit();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute reinit(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
    }
    // Read out information about the sensor
    error = sensor.getSerialNumber(serialNumber);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    Serial.print("serial number: 0x");
    Serial.println(serialNumber);
}