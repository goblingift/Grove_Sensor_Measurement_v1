#define LED_BUILTIN 21  // Onboard LED is connected to GPIO21 on XIAO ESP32S3
#define HUMIDIFIER A1

void setup() {
  Serial.begin(115200);        // Start serial communication at 115200 baud rate
  pinMode(LED_BUILTIN, OUTPUT); // Set the LED pin as an output
  pinMode(HUMIDIFIER, OUTPUT);
  Serial.println("Hello from XIAO ESP32S3!"); // Print message once on start
}

void loop() {
  Serial.println("Power on Humidifier for 30 seconds");
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(HUMIDIFIER, HIGH);
  delay(10000);
  digitalWrite(LED_BUILTIN, LOW);
  digitalWrite(HUMIDIFIER, LOW);
  Serial.println("Sleep for 5 seconds");
  delay(5000);
}
