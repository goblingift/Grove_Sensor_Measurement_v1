#include <U8g2lib.h>
#include <Wire.h>
#include <INA226_WE.h>

// HW I2C constructor - shares Wire cleanly
U8G2_SH1107_SEEED_128X128_F_HW_I2C u8g2(U8G2_R0, /* reset=*/ U8X8_PIN_NONE);

INA226_WE ina226 = INA226_WE(0x40);

void setup() {
  Serial.begin(9600);
  Wire.begin();

  // INA226 FIRST
  if (!ina226.init()) {
    Serial.println("INA226 not found!");
  } else {
    Serial.println("INA226 OK!");
    ina226.setAverage(INA226_AVERAGE_16);
    ina226.setConversionTime(INA226_CONV_TIME_1100);
    ina226.setMeasureMode(INA226_CONTINUOUS);
    ina226.waitUntilConversionCompleted();
  }

  // OLED SECOND
  u8g2.begin();
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x12_tr);
    u8g2.drawStr(0, 24, "Incubator started!");
  } while (u8g2.nextPage());
  delay(2000);

  Serial.println("Setup complete!");
}

void loop() {
  // INA226 voltage
  float voltage = ina226.getBusVoltage_V();
  
  // Serial
  Serial.print("Voltage: ");
  Serial.print(voltage, 2);
  Serial.println(" V");

  // OLED: Convert float to char array
  char voltageStr[10];
  dtostrf(voltage, 6, 2, voltageStr);  // 6 chars total, 2 decimals

  // Clear and draw
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tr);
  u8g2.drawStr(0, 16, "Voltage Monitor");
  u8g2.drawStr(0, 40, "V = ");
  u8g2.drawStr(40, 40, voltageStr);
  u8g2.drawStr(80, 40, "V");
  u8g2.sendBuffer();  // Full buffer mode

  delay(500);  // Update every 0.5s
}