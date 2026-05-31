#include <Arduino.h>

// LED on A0 (GPIO1) - XIAO ESP32-S3
// Pattern: long ON, short ON, repeat

#define LED_PIN 1  // A0 = GPIO1 on XIAO ESP32-S3

#define LONG_ON   800
#define SHORT_ON  200
#define PAUSE     300
#define CYCLE_GAP 1200

void setup() {
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  // Long blink
  digitalWrite(LED_PIN, HIGH);
  delay(LONG_ON);
  digitalWrite(LED_PIN, LOW);
  delay(PAUSE);

  // Short blink
  digitalWrite(LED_PIN, HIGH);
  delay(SHORT_ON);
  digitalWrite(LED_PIN, LOW);
  delay(CYCLE_GAP);
}
