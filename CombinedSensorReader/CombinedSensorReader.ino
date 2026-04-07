#include "HX711.h"
#include <Preferences.h>
#include <math.h>
#include <Arduino.h>
#include <SensirionI2cScd4x.h>
#include <Wire.h>

// pins
const int HX711_DT_PIN   = 3;
const int HX711_SCK_PIN  = 2;
const int pinLightSensor = A0;



// Wio-SX1262 user button
const int USER_BUTTON_PIN = 21;

// Known reference weight in grams
const float REFERENCE_WEIGHT_GRAMS = 704.0f;

// Fallback calibration factor if nothing valid is stored yet
const float DEFAULT_CALIBRATION_FACTOR = 1.0f;

SensirionI2cScd4x sensorSCD41;
HX711 scale;
Preferences preferences;
float calibrationFactor = DEFAULT_CALIBRATION_FACTOR;
int lightIntensity = 0;
uint16_t co2 = 0;
float temp = 0.0;
float rh = 0.0;

// Button debounce / edge detection
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

static int16_t error;
static char errorMessage[64];

#ifdef NO_ERROR
#undef NO_ERROR
#endif
#define NO_ERROR 0

void saveCalibrationFactor() {
  preferences.begin("scale", false);
  preferences.putFloat("calFactor", calibrationFactor);
  preferences.end();

  Serial.print("Saved calibration factor: ");
  Serial.println(calibrationFactor, 6);
}

void loadCalibrationFactor() {
  preferences.begin("scale", true);
  calibrationFactor = preferences.getFloat("calFactor", DEFAULT_CALIBRATION_FACTOR);
  preferences.end();

  if (!isfinite(calibrationFactor) || calibrationFactor == 0.0f) {
    calibrationFactor = DEFAULT_CALIBRATION_FACTOR;
    Serial.println("Stored calibration factor invalid. Using default.");
  }

  Serial.print("Loaded calibration factor: ");
  Serial.println(calibrationFactor, 6);
}

void tareScale() {
  Serial.println("Tare... remove any weights from the scale.");
  delay(5000);
  scale.tare(10);
  Serial.println("Tare done.");
}

void calibrateScale() {
  if (!scale.is_ready()) {
    Serial.println("HX711 not found. Calibration aborted.");
    return;
  }

  Serial.println("Calibration started.");

  scale.set_scale();   // reset scale factor to 1
  tareScale();

  Serial.print("Place reference weight on the scale: ");
  Serial.print(REFERENCE_WEIGHT_GRAMS, 1);
  Serial.println(" g");
  delay(5000);

  float measuredValue = scale.get_units(10);

  if (REFERENCE_WEIGHT_GRAMS == 0.0f) {
    Serial.println("Reference weight must not be 0.");
    return;
  }

  calibrationFactor = measuredValue / REFERENCE_WEIGHT_GRAMS;

  if (!isfinite(calibrationFactor) || calibrationFactor == 0.0f) {
    Serial.println("Calculated calibration factor invalid. Not saved.");
    return;
  }

  scale.set_scale(calibrationFactor);
  saveCalibrationFactor();

  Serial.print("Measured value: ");
  Serial.println(measuredValue, 3);
  Serial.print("New calibration factor: ");
  Serial.println(calibrationFactor, 6);
  Serial.println("Calibration finished.");
}

void handleCalibrationButton() {
  bool currentButtonState = digitalRead(USER_BUTTON_PIN);

  if (currentButtonState != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    static bool buttonHandled = false;

    if (currentButtonState == LOW && !buttonHandled) {
      Serial.println("Calibration button pressed.");
      calibrateScale();
      buttonHandled = true;
    }

    if (currentButtonState == HIGH) {
      buttonHandled = false;
    }
  }

  lastButtonState = currentButtonState;
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
  sensorSCD41.begin(Wire, SCD41_I2C_ADDR_62);
  uint64_t serialNumber = 0;
  delay(30);

  error = sensorSCD41.wakeUp();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute wakeUp(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
    }
    error = sensorSCD41.stopPeriodicMeasurement();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute stopPeriodicMeasurement(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
    }
    error = sensorSCD41.reinit();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute reinit(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
    }
    // Read out information about the sensor
    error = sensorSCD41.getSerialNumber(serialNumber);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute getSerialNumber(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }
    Serial.print("serial number: 0x");
    Serial.println(serialNumber);
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  initializeSCD41();
  Serial.println("CO2, Temp, Humidity Sensor starts work!");

  Serial.println("Initializing scale!");
  pinMode(USER_BUTTON_PIN, INPUT_PULLUP);
  scale.begin(HX711_DT_PIN, HX711_SCK_PIN);

  if (!scale.is_ready()) {
    Serial.println("HX711 not found.");
    return;
  }

  Serial.println("HX711 ready on XIAO ESP32-S3");

  loadCalibrationFactor();
  scale.set_scale(calibrationFactor);
  scale.tare(10);

  Serial.println("Scale initialized with stored calibration factor.");
  Serial.println("Press the user button to recalibrate.");
}

void loop() {
  handleCalibrationButton();

  if (scale.is_ready()) {
    float oneReading = scale.get_units(1);
    float averageReading = scale.get_units(10);

    Serial.print("one reading:");
    Serial.print(oneReading, 1);
    Serial.print(" grams| average:");
    Serial.println(averageReading, 1);
  } else {
    Serial.println("HX711 not found.");
  }

  error = sensorSCD41.wakeUp();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute wakeUp(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }

    // Trigger first measurement after wake up, but ignore values
    error = sensorSCD41.measureSingleShot();
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute measureSingleShot(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    }

    error = sensorSCD41.measureAndReadSingleShot(co2, temp, rh);
    if (error != NO_ERROR) {
        Serial.print("Error trying to execute measureAndReadSingleShot(): ");
        errorToString(error, errorMessage, sizeof errorMessage);
        Serial.println(errorMessage);
        return;
    } else {
      printMeasurement(co2, temp, rh);
    }

    lightIntensity = analogRead(pinLightSensor);
    Serial.print("Measured light-sensity:");
    Serial.println(lightIntensity);

  delay(1000);
}