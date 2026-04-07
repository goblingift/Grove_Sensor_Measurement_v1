#include "HX711.h"
#include <Preferences.h>
#include <math.h>

// HX711 pins
const int HX711_DT_PIN   = 3;
const int HX711_SCK_PIN  = 2;

// Wio-SX1262 user button
const int USER_BUTTON_PIN = 21;

// Known reference weight in grams
const float REFERENCE_WEIGHT_GRAMS = 704.0f;

// Fallback calibration factor if nothing valid is stored yet
const float DEFAULT_CALIBRATION_FACTOR = 1.0f;

HX711 scale;
Preferences preferences;

float calibrationFactor = DEFAULT_CALIBRATION_FACTOR;

// Button debounce / edge detection
bool lastButtonState = HIGH;
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50;

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

void setup() {
  Serial.begin(115200);
  delay(1000);
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
    Serial.print(" | average:");
    Serial.println(averageReading, 1);
  } else {
    Serial.println("HX711 not found.");
  }

  delay(1000);
}