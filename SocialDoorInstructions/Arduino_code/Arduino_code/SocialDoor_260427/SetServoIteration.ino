// Created 4/27/2026
// This code allows you to change servo iteration times when the door opens and closes. Recommendation: 4-5 for "Open"; 3 for "Close".
// Low battery or bad servo might cause  to remain unopened or unclosed; increasing iteration values can give the servo more time to push the "door switch" on.
// Old/broken circuit board or bad soldering might cause slow looping of read_sensor() during servo operation, thus leading to over-opening and/or over-closing and damaging servo.
// Adjusting the servo iteration values can reduce this problem while making sure the door open/close completely.
// Press button B to choose "Close" value; Press button C to choose "Open" value. Both parameters range from 6 to 1 (0 is not recommended).
// If over-opening/-closing still occurr with a value 1, please go to OpenDoor.ino and change myservo.write(xx) to slow down servo operation. For opening, lower the value (must > 95). For closing, increase the value (must < 85).

void set_servo_iternum() {
  // create files if they dont exist and grab device name and ratio
  servoopen = SD.open("ServoOpening.csv", FILE_WRITE);
  servoopen = SD.open("ServoOpening.csv", FILE_READ);
  OP_i = servoopen.parseInt();
  servoopen.close();

  servoclose = SD.open("ServoClosing.csv", FILE_WRITE);
  servoclose = SD.open("ServoClosing.csv", FILE_READ);
  CL_i = servoclose.parseInt();
  servoclose.close();

  unsigned long functiontime = millis();
  while (millis() - functiontime < 3000) {
    display.clearDisplay();
    display.display();
    delay(100);

    ReadBatteryLevel();
    DisplayBattery();
    display.setCursor(21, 2);
    display.print(measuredvbat);
    display.setCursor(0, 15);
    display.println("Open (C)");
    display.println(OP_i);
    display.println("Close (B)");
    display.println(CL_i);
    display.display();
    delay(100);

    if (digitalRead(6) == LOW) {
      CL_i--;
      delay(50);
      functiontime = millis();
      if (CL_i < 1) {
        CL_i = 6;
      }
    }

    if (digitalRead(5) == LOW) {
      OP_i--;
      delay(50);
      functiontime = millis();
      if (OP_i < 1) {
        OP_i = 6;
      }
    }
  }
  writeConfigFile2();
  writeConfigFile3();
}