const int pinLightSensor = A0;
int lightIntensity = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Light sensor starts work!");
}

void loop() {
  
  lightIntensity = analogRead(pinLightSensor);
  Serial.print("Measured light-sensity:");
  Serial.println(lightIntensity);
  delay(1000);
}
