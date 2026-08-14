// This tab has been updated to call Menu at the starting stage since 8/13/2026.
void StartUpCommands() {
  Serial.begin(115200);
  Serial.println("Starting up...");

  Wire.begin();
  SPI.begin();
  rtc.begin();

  delay(1000);

  display.begin(0x3C, true);  // Address 0x3C default
  delay(1000);

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SH110X_WHITE);
  display.setRotation(0);
  display.setCursor(0, 0);
  display.setContrast(0);  // dim display
  display.display();

  pinMode(SHT_PROX2, OUTPUT);
  pinMode(11, OUTPUT);          // Servo power
  pinMode(10, OUTPUT);          // MR1
  pinMode(12, INPUT_PULLDOWN);  // 3.5mm Input
  pinMode(5, INPUT_PULLUP);     // Button C
  pinMode(6, INPUT_PULLUP);     // Button B
  pinMode(9, INPUT_PULLUP);     // Button A but doesn't work
  pinMode(8, OUTPUT);           // Green LED
  pinMode(13, OUTPUT);          // Red LED
  pinMode(A1, OUTPUT);          // This output is for T1.
  pinMode(A2, OUTPUT);          // This output is for T2.
  pinMode(A3, OUTPUT);          // Existing behavior retained
  digitalWrite(A2, LOW);
  digitalWrite(A3, HIGH);
  digitalWrite(13, LOW);

  LowPower.attachInterruptWakeup(12, WakeUp, CHANGE);
  LowPower.attachInterruptWakeup(5, WakeUp, CHANGE);
  LowPower.attachInterruptWakeup(6, WakeUp, CHANGE);

  /********************************************************
    Start SD card
  ********************************************************/
  if (!SD.begin(chipSelect, SD_SCK_MHZ(4))) {
    Serial.println("Card failed, or not present");
    error();
  }

  Serial.println("card initialized.");

  // IMPORTANT CHANGE:
  // Always LOAD settings, but do not force the user through edit screens.
  loadSavedSettings();

  // Show the startup page for 3 seconds.
  // Holding BOTH B + C for >= 600 ms opens the settings menu.
  // Otherwise this returns after 3 seconds and setup continues normally.
  if (startupMenuRequested()) {
    runSettingsMenu();
  }

  screen_time = millis();
  update_display();

  //////////////////////////////////////////////////////////////////////////
  // Generate a unique filename AFTER the menu, so changes to device number
  // or RTC time are reflected in the new filename.
  //////////////////////////////////////////////////////////////////////////
  int n = 0;
  DateTime now = rtc.now();

  SdFile::dateTimeCallback(dateTime);

  snprintf(filename, sizeof(filename),
           "DOOR%03d_%02d%02d%02d_%02d.csv",
           SOC, now.month(), now.day(), now.year() - 2000, n);

  while (SD.exists(filename)) {
    n++;
    snprintf(filename, sizeof(filename),
             "DOOR%03d_%02d%02d%02d_%02d.csv",
             SOC, now.month(), now.day(), now.year() - 2000, n);
  }

  Serial.print("New file created: ");
  Serial.println(filename);

  writeHeader();
  Serial.println("Files fine");

  /********************************************************
    Set I2C addresses for the prox sensors
  ********************************************************/
  setI2C();

  /********************************************************
    Toggle door
  ********************************************************/
  button = 414;
  open_door_startup();
  close_door();
}