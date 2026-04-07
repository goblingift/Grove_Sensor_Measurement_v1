#include "HX711.h"

// Connect the grove cable of HX711 board to Grove shield: D1/A1
const int HX711_DT_PIN  = 3;   // ESP32S3 GPIO3/D2/A2
const int HX711_SCK_PIN = 2;   // ESP32S3 GPIO2/D1/A1

HX711 scale;

// Known reference weight in grams
const float REFERENCE_WEIGHT_GRAMS = 704.0f;

// Global calibration factor
float calibrationFactor = 1.0f;

void tareScale() {
  Serial.println("Tare... remove any weights from the scale.");
  delay(5000);
  scale.tare(10);
  Serial.println("Tare done.");
}

void calibrateScale() {
  Serial.println("Calibration started.");
  
  scale.set_scale();  // reset scale factor to 1.0
  tareScale();

  Serial.print("Place reference weight on the scale: ");
  Serial.print(REFERENCE_WEIGHT_GRAMS, 1);
  Serial.println(" g");
  delay(5000);

  float measuredValue = scale.get_units(10);
  calibrationFactor = measuredValue / REFERENCE_WEIGHT_GRAMS;

  scale.set_scale(calibrationFactor);

  Serial.print("Measured raw calibrated units: ");
  Serial.println(measuredValue, 3);
  Serial.print("Calibration factor: ");
  Serial.println(calibrationFactor, 6);

  Serial.println("Calibration finished.");
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("Initializing scale!");

  scale.begin(HX711_DT_PIN, HX711_SCK_PIN);

  if (scale.is_ready()) {
    Serial.println("HX711 ready on XIAO ESP32-S3");
    calibrateScale();
  } else {
    Serial.println("HX711 not found.");
  }
}

void loop() {
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