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
  pinMode(11, OUTPUT);  //Servo power
  pinMode(10, OUTPUT);  //MR1
  pinMode(12, INPUT_PULLDOWN);
  pinMode(5, INPUT_PULLUP);
  pinMode(6, INPUT_PULLUP);  // Button
  pinMode(9, INPUT_PULLUP);  // Button
  pinMode(8, OUTPUT);
  pinMode(13, OUTPUT);
  pinMode(A1, OUTPUT);  //This output is for a triggered camera with Bonsai
  pinMode(A2, OUTPUT);  // This output is for T2.
  pinMode(A3, OUTPUT);  //THIS DOES NOT MAKE SENSE!!! SHOULD BE AN INPUT_PULLUP!
  digitalWrite(A1, LOW);
  digitalWrite(A2, LOW);
  digitalWrite(A3, HIGH);  //THIS DOES NOT MAKE SENSE!!! SHOULD BE AN INPUT_PULLUP!
  digitalWrite(13, LOW);

  /********************************************************
    Start SD card, create Data File for current session
  ********************************************************/
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect, SD_SCK_MHZ(4))) {
    Serial.println("Card failed, or not present");
    error();
  }
  Serial.println("card initialized.");

  // IMPORTANT CHANGE:
  // Always LOAD settings, but do not force the user through edit screens.
  loadSavedSettings();

  // Keep returning to the startup menu after settings.
  // If B+C is NOT pressed during the 3-second window,
  // continue with normal program setup.
  while (startupMenuRequested()) {

    runSettingsMenu();

    // Reload anything that may have changed in the settings menu
    loadSavedSettings();
  }

  update_display();

  //////////////////////////////////////////////////////////////////////////
  // Generate a unique filename
  // (code below from: https://forum.arduino.cc/index.php?topic=372248.0)
  //////////////////////////////////////////////////////////////////////////
  int n = 0;
  DateTime now = rtc.now();

  // Initialize SD card and create the datafile
  SdFile::dateTimeCallback(dateTime);

  snprintf(filename, sizeof(filename), "DOOR%03d_%02d%02d%02d_%02d.csv", SOC, now.month(), now.day(), now.year() - 2000, n);
  while (SD.exists(filename)) {
    n++;
    snprintf(filename, sizeof(filename), "DOOR%03d_%02d%02d%02d_%02d.csv", SOC, now.month(), now.day(), now.year() - 2000, n);
  }

  Serial.print("New file created: ");
  Serial.println(filename);

  writeHeader();
  Serial.println("Files fine");

  /********************************************************
    Set i2C addresses for the prox sensors
  ********************************************************/
  setI2C();

  /********************************************************
    Toggle door
  ********************************************************/
  open_door_startup();
  close_door();
}
