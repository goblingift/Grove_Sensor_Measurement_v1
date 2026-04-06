#include "HX711.h"
// Connect the grove cable of HX711 board to Grove shield: D1/A1
const int HX711_DT_PIN  = 3;   // ESP32S3 GPIO3/D2/A2
const int HX711_SCK_PIN = 2;   // ESP32S3 GPIO2/D1/A1

HX711 scale;

void setup() {
  Serial.println("Initializing scale!");
  Serial.begin(115200);

  // Initialize HX711
  scale.begin(HX711_DT_PIN, HX711_SCK_PIN);

  // Optional: set calibration factor later
  scale.set_scale();           // default scale
  scale.tare();                // zero baseline

  Serial.println("HX711 via Grove D1 port ready on XIAO ESP32‑S3");
}

void loop() {
  if (scale.is_ready()) {
    float weight = scale.get_units(5);   // average 5 readings
    Serial.print("Weight: ");
    Serial.print(weight, 1);
    Serial.println(" g");
  } else {
    Serial.println("HX711 not ready");
  }

  delay(1000);
}