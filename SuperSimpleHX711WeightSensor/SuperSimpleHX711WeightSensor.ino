#include "HX711.h"

const int DT_PIN  = 3;  // ESP32S3 GPIO3/D2/A2
const int SCK_PIN = 2;  // ESP32S3 GPIO2/D1/A1

HX711 scale;

const float KNOWN_WEIGHT_GRAMS = 1638.0;

void waitForEnter() {
  while (Serial.available()) Serial.read();
  while (!Serial.available()) { delay(10); }
  while (Serial.available()) Serial.read();
}

void setup() {
  Serial.begin(9600);
  delay(1000);

  Serial.println("\n=== HX711 Calibration (ESP32S3) ===");
  Serial.print("DT="); Serial.print(DT_PIN); Serial.println(" (D2/A2)");
  Serial.print("SCK="); Serial.print(SCK_PIN); Serial.println(" (D1/A1)");

  scale.begin(DT_PIN, SCK_PIN);  // Your library API

  Serial.println("\n1) Remove all weight, then press ENTER");
  waitForEnter();

  scale.tare(20);  // Zero empty scale
  Serial.println("Tare done.");

  Serial.println("\n2) Put the 1638 g test weight on the scale, then press ENTER");
  waitForEnter();

  // Raw value = average - tare offset
  float rawValue = scale.read_average(20) - scale.get_offset();
  float calibrationFactor = rawValue / KNOWN_WEIGHT_GRAMS;

  Serial.println("\n=== RESULTS ===");
  Serial.print("Raw value: "); Serial.println(rawValue, 0);
  Serial.print("Calibration factor: "); Serial.println(calibrationFactor, 8);

  scale.set_scale(calibrationFactor);  // Apply calibration
  Serial.println("Calibration saved!");

  Serial.println("\n3) Remove test weight, press ENTER for live mode");
  waitForEnter();
  Serial.println("Live readings (g):");
}

void loop() {
  float weight = scale.get_units(10);  // Calibrated weight, 10x average

  Serial.print("Weight: ");
  Serial.print(weight, 1);
  Serial.println(" g");

  delay(1000);
}