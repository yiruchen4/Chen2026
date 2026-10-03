// Write data header to file of uSD.
void writeHeader() {
  logfile = SD.open(filename, FILE_WRITE);
  logfile.println("Datetime,Seconds,Device_Number,Battery_Voltage,Prox1,Prox2,Event,Door,Opto");
  logfile.close();
}

// Print data and time followed by pellet count and motorturns to SD card
void logdata() {
  if (!SD.begin(chipSelect, SD_SCK_MHZ(4))) {
    Serial.println("Card failed, or not present");
    error();
  }

  digitalWrite(13, HIGH);
  unsigned long timer = millis();
  DateTime now = rtc.now();
  digitalWrite(13, LOW);
  // getFilename(filename);
  logfile = SD.open(filename, FILE_WRITE);
  logfile.print(now.month());
  logfile.print('/');
  logfile.print(now.day());
  logfile.print('/');
  logfile.print(now.year());
  logfile.print(" ");
  logfile.print(now.hour());
  logfile.print(':');
  logfile.print(now.minute());
  logfile.print(':');
  logfile.print(now.second());
  logfile.print(",");
  logfile.print((timer - new_trial) / 1000.000, 3);  // Print time in seconds
  logfile.print(",");
  logfile.print(SOC);  // Print device name
  logfile.print(",");
  logfile.print(measuredvbat);
  logfile.print(",");
  logfile.print(Range1);  // Print range1
  logfile.print(",");
  logfile.print(Range2);  //print range2
  logfile.print(",");
  logfile.print(open_num);  //print event
  logfile.print(",");
  logfile.print(door);
  logfile.print(",");
  logfile.println(opto);  //print door situation


  if (!logfile) {
    error();
  }
  logfile.close();

  //Fix data rate at 10hz
  while ((millis() - startreading) < 124) {
    delay(1);
  }
}

void writeConfigFile() {
  configfile = SD.open("DoorNumber.csv", FILE_WRITE);
  configfile.rewind();
  configfile.println(SOC);
  configfile.flush();
  configfile.close();
}

void writeConfigFile2() {
  servoopen = SD.open("ServoOpening.csv", FILE_WRITE);
  servoopen.rewind();
  servoopen.println(OP_i);
  servoopen.flush();
  servoopen.close();
}

void writeConfigFile3() {
  servoclose = SD.open("ServoClosing.csv", FILE_WRITE);
  servoclose.rewind();
  servoclose.println(CL_i);
  servoclose.flush();
  servoclose.close();
}

void loadOptoWindows() {

  File file = SD.open("OptoWindows.csv", FILE_READ);

  // If no file exists, keep the defaults from a_Header.h
  if (!file) {
    Serial.println("No OptoWindows.csv - using defaults");
    return;
  }

  for (int i = 0; i < NUM_OPTO_WINDOWS; i++) {

    if (!file.available()) {
      break;
    }

    int startValue = file.parseInt();

    // Move past comma
    if (file.peek() == ',') {
      file.read();
    }

    int endValue = file.parseInt();

    // Make sure values are valid minutes of the day
    if ((startValue >= 0) && (startValue < 1440) && (endValue >= 0) && (endValue < 1440)) {

      optoWindows[i].startTime = startValue;
      optoWindows[i].endTime = endValue;
    }

    // Move through the rest of the line
    while (file.available()) {

      char c = file.read();

      if (c == '\n') {
        break;
      }
    }
  }

  file.close();
}

void writeOptoWindows() {

  // Remove old file so we don't accidentally leave old data behind
  if (SD.exists("OptoWindows.csv")) {
    SD.remove("OptoWindows.csv");
  }

  File file = SD.open("OptoWindows.csv", FILE_WRITE);

  if (!file) {
    Serial.println("Could not write OptoWindows.csv");
    return;
  }

  for (int i = 0; i < NUM_OPTO_WINDOWS; i++) {

    file.print(optoWindows[i].startTime);
    file.print(",");
    file.println(optoWindows[i].endTime);
  }

  file.flush();
  file.close();
}