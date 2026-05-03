#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <MQTTClient.h>
#include <ArduinoJson.h>
#include "config.h"

WiFiClientSecure net = WiFiClientSecure();
MQTTClient client(512);

const int LED_PIN = D1;

void messageHandler(String &topic, String &payload) {
  Serial.println("Incoming topic: " + topic);
  Serial.println("Payload: " + payload);

  JsonDocument doc;
  DeserializationError err = deserializeJson(doc, payload);
  if (err) {
    Serial.println("Invalid JSON command");
    return;
  }

  const char* led = doc["led"];
  if (led) {
    if (String(led) == "on") {
      digitalWrite(LED_PIN, HIGH);
      publishStatus("led_on");
    } else if (String(led) == "off") {
      digitalWrite(LED_PIN, LOW);
      publishStatus("led_off");
    }
  }
}

void publishStatus(const char* state) {
  JsonDocument doc;
  doc["device_id"] = THINGNAME;
  doc["status"] = state;

  char jsonBuffer[256];
  serializeJson(doc, jsonBuffer);
  client.publish(AWS_IOT_STATUS_TOPIC, jsonBuffer);
}

void connectAWS() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  Serial.println("WiFi connected");
  Serial.println("IP address: " + WiFi.localIP().toString());

  // Test DNS resolution
  IPAddress resolvedIP;
  if (WiFi.hostByName(AWS_IOT_ENDPOINT, resolvedIP)) {
    Serial.println("DNS resolved: " + resolvedIP.toString());
  } else {
    Serial.println("DNS resolution failed");
    return;
  }

  // Load certificates
  Serial.println("Loading certificates...");
  net.setCACert(AWS_CERT_CA);
  Serial.println("CA loaded");
  net.setCertificate(AWS_CERT_CRT);
  Serial.println("Device cert loaded");
  net.setPrivateKey(AWS_CERT_PRIVATE);
  Serial.println("Private key loaded");

  client.begin(AWS_IOT_ENDPOINT, 8883, net);
  client.onMessage(messageHandler);

  Serial.print("Connecting to AWS IoT as ");
  Serial.println(THINGNAME);
  Serial.println("Publish topic: " + String(AWS_IOT_PUBLISH_TOPIC));
  Serial.println("Subscribe topic: " + String(AWS_IOT_SUBSCRIBE_TOPIC));

  int connectAttempts = 0;
  while (!client.connect(THINGNAME) && connectAttempts < 10) {
    Serial.println("MQTT connect failed, retrying...");
    delay(2000);
    connectAttempts++;
  }

  if (!client.connected()) {
    Serial.println("MQTT connection failed permanently");
    Serial.println("Check: endpoint, certificates, IoT policy");
    return;
  }

  Serial.println("AWS IoT connected");
  client.subscribe(AWS_IOT_SUBSCRIBE_TOPIC);
}

void publishTelemetry() {
  JsonDocument doc;
  doc["device_id"] = THINGNAME;
  doc["temperature_celsius"] = 37.2;
  doc["humidity_rh"] = 58.4;
  doc["co2_ppm"] = 810;
  doc["light_intensity"] = 220;
  doc["gyroscope_x"] = 0.1;
  doc["gyroscope_y"] = 0.0;
  doc["gyroscope_z"] = 0.0;
  doc["sound_intensity"] = 35.0;
  doc["weight_gram"] = 2400;

  char jsonBuffer[512];
  serializeJson(doc, jsonBuffer);
  
  Serial.print("Publishing to ");
  Serial.print(AWS_IOT_PUBLISH_TOPIC);
  Serial.print(": ");
  Serial.println(jsonBuffer);

  if (!client.connected()) {
    Serial.print("Publish skipped, MQTT not connected. lastError=");
    Serial.print(client.lastError());
    Serial.print(" returnCode=");
    Serial.println(client.returnCode());
    return;
  }

  bool ok = client.publish(AWS_IOT_PUBLISH_TOPIC, jsonBuffer);
  if (ok) {
    Serial.println("Publish accepted by client (queued/sent to broker)");
  } else {
    Serial.print("Publish FAILED. lastError=");
    Serial.print(client.lastError());
    Serial.print(" returnCode=");
    Serial.print(client.returnCode());
    Serial.print(" connected=");
    Serial.println(client.connected() ? "true" : "false");
  }
}

void setup() {
  Serial.begin(115200);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
  connectAWS();
}

void loop() {
  client.loop();

  static bool wasConnected = true;
  bool nowConnected = client.connected();
  if (wasConnected && !nowConnected) {
    Serial.print("MQTT session lost. lastError=");
    Serial.print(client.lastError());
    Serial.print(" returnCode=");
    Serial.println(client.returnCode());
  }
  wasConnected = nowConnected;

  static unsigned long lastPublish = 0;
  if (millis() - lastPublish > 10000) {
    publishTelemetry();
    lastPublish = millis();
  }
}