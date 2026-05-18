#define ATOMIZER_PIN A9

void setup() {
  pinMode(ATOMIZER_PIN, OUTPUT);
  digitalWrite(ATOMIZER_PIN, LOW);
}

void loop() {
  digitalWrite(ATOMIZER_PIN, HIGH);  // atomizer ON
  delay(10000);                      // 10 seconds

  digitalWrite(ATOMIZER_PIN, LOW);   // atomizer OFF
  delay(5000);                       // 5 seconds
}