#include <Wire.h>
#include <ADXL345.h>
#include <math.h>

ADXL345 adxl;

void setup() {
  Serial.begin(9600);
  adxl.powerOn();
}

void loop() {
  double xyz[3];
  adxl.getAcceleration(xyz);

  float ax = xyz[0];
  float ay = xyz[1];
  float az = xyz[2];

  float pitch = atan2(ax, sqrt(ay * ay + az * az)) * 180.0 / PI;
  float roll  = atan2(ay, sqrt(ax * ax + az * az)) * 180.0 / PI;

  Serial.print("Pitch = ");
  Serial.print(pitch, 2);
  Serial.println(" deg");

  Serial.print("Roll  = ");
  Serial.print(roll, 2);
  Serial.println(" deg");

  delay(500);
}