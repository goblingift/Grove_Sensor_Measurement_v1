const int pinAdc = A8;

TaskHandle_t soundTaskHandle = NULL;

void soundRecorderTask(void *parameter) {
  for (;;) {
    const unsigned long durationMs = 10000;   // 10 seconds
    const unsigned long startMs = millis();
    int highestLoudness = 0;

    while (millis() - startMs < durationMs) {
      long sum = 0;

      for (int i = 0; i < 32; i++) {
        sum += analogRead(pinAdc);
      }

      sum >>= 5;   // divide by 32

      if (sum > highestLoudness) {
        highestLoudness = sum;
      }

      vTaskDelay(pdMS_TO_TICKS(10));  // small pause, allows scheduler to run other tasks
    }

    Serial.print("Highest loudness in last 10s: ");
    Serial.println(highestLoudness);

    // next 10-second measurement starts immediately
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  xTaskCreatePinnedToCore(
    soundRecorderTask,   // task function
    "SoundRecorder",     // task name
    4096,                // stack size
    NULL,                // parameter
    1,                   // priority
    &soundTaskHandle,    // task handle
    1                    // core
  );
}

void loop() {
  // Keep loop empty when FreeRTOS task does the work
  vTaskDelay(pdMS_TO_TICKS(1000));
}