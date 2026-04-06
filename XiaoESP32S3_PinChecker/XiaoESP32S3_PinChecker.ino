const int HX711_DT_PIN  = 3;   // ESP32S3 GPIO3 / D2 / A2
const int HX711_SCK_PIN = 2;   // ESP32S3 GPIO2 / D1 / A1

void setup() {
  Serial.begin(115200);

  pinMode(HX711_DT_PIN, OUTPUT);
  pinMode(HX711_SCK_PIN, OUTPUT);

  digitalWrite(HX711_DT_PIN, LOW);
  digitalWrite(HX711_SCK_PIN, LOW);
}

void loop() {
  Serial.println("Before HX711_DT_PIN HIGH");
  digitalWrite(HX711_DT_PIN, HIGH);
  delay(2000);
  Serial.println("After HX711_DT_PIN HIGH");
  digitalWrite(HX711_DT_PIN, LOW);

  delay(300);

  Serial.println("Before HX711_SCK_PIN HIGH");
  digitalWrite(HX711_SCK_PIN, HIGH);
  delay(2000);
  Serial.println("After HX711_SCK_PIN HIGH");
  digitalWrite(HX711_SCK_PIN, LOW);

  delay(5000);
}