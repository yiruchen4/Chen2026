// Date and time functions using a PCF8523 RTC connected via I2C and Wire lib
#include "RTClib.h"
RTC_PCF8523 rtc;

void setup() {
  Serial.begin(57600);
  rtc.begin();
  rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  //set with computer time
}

void loop() {
  DateTime now = rtc.now();

  Serial.print(now.year(), DEC);
  Serial.print('/');
  Serial.print(now.month(), DEC);
  Serial.print('/');
  Serial.print(now.day(), DEC);
  Serial.print(" ");
  Serial.print(now.hour(), DEC);
  Serial.print(':');
  Serial.print(now.minute(), DEC);
  Serial.print(':');
  Serial.print(now.second(), DEC);
  Serial.println();
  delay(3000);
}
