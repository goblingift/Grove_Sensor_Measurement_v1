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
  Serial.println("HX711 ready on XIAO ESP32‑S3");
}

void loop() {
  if (scale.is_ready()) {
    scale.set_scale();    
    Serial.println("Tare... remove any weights from the scale.");
    delay(5000);
    scale.tare();
    Serial.println("Tare done...");
    Serial.print("Place a known weight on the scale...");
    delay(5000);
    long reading = scale.get_units(10);
    Serial.print("Result: ");
    Serial.println(reading);
  } 
  else {
    Serial.println("HX711 not found.");
  }

  delay(10000);
}