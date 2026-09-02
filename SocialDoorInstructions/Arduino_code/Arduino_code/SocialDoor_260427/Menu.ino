// Startup settings sequence
// Button B = pin 6
// Button C = pin 5
// Buttons are active LOW because INPUT_PULLUP is used.
//
// Startup behavior:
//   - Startup screen is shown for 3 seconds.
//   - Hold B + C for 600 ms during that window to enter settings.
//   - If settings are entered, pages run automatically in this order:
//       1. Device number      (existing set_device_num())
//       2. Servo iterations   (existing set_servo_iternum())
//       3. RTC time           (set_rtc_time() below)
//   - No B+C save/enter action is used inside any settings page.
//   - Device, Servo, and RTC each advance after 3 seconds of inactivity.

const uint8_t BUTTON_C = 5;
const uint8_t BUTTON_B = 6;

const unsigned long MENU_START_WINDOW_MS = 3000;
const unsigned long MENU_HOLD_MS = 50;
const unsigned long RTC_INACTIVITY_MS = 3000;


// -----------------------------------------------------------------------------
// Wait until the startup B+C combination has been released.
// This prevents the same button hold from immediately changing Device #.
// -----------------------------------------------------------------------------
void waitForMenuButtonsRelease() {
  while ((digitalRead(BUTTON_B) == LOW) ||
         (digitalRead(BUTTON_C) == LOW)) {
    delay(10);
  }
  delay(40);  // debounce
}


// -----------------------------------------------------------------------------
// Print a number with a leading zero when needed.
// -----------------------------------------------------------------------------
void menuPrint2Digits(int value) {
  if (value < 10) {
    display.print('0');
  }
  display.print(value);
}


// -----------------------------------------------------------------------------
// Load the saved values without showing their editing screens.
// This is called every startup so SOC, OP_i, and CL_i are available even when
// the user does NOT enter the settings sequence.
// -----------------------------------------------------------------------------
void loadSavedSettings() {
  // Device number
  SOC = 0;
  configfile = SD.open("DoorNumber.csv", FILE_READ);
  if (configfile) {
    int value = configfile.parseInt();
    if (value >= 0 && value <= 999) {
      SOC = value;
    }
    configfile.close();
  }

  // Servo opening iterations
  OP_i = 4;
  servoopen = SD.open("ServoOpening.csv", FILE_READ);
  if (servoopen) {
    int value = servoopen.parseInt();
    if (value >= 1 && value <= 6) {
      OP_i = value;
    }
    servoopen.close();
  }

  // Servo closing iterations
  CL_i = 3;
  servoclose = SD.open("ServoClosing.csv", FILE_READ);
  if (servoclose) {
    int value = servoclose.parseInt();
    if (value >= 1 && value <= 6) {
      CL_i = value;
    }
    servoclose.close();
  }
}


// -----------------------------------------------------------------------------
// 3-second startup window.
// Return true only when B + C are held together for MENU_HOLD_MS.
// -----------------------------------------------------------------------------
bool startupMenuRequested() {
  unsigned long startTime = millis();
  unsigned long bothPressedStart = 0;

  while (millis() - startTime < MENU_START_WINDOW_MS) {
    display.oled_command(SH110X_DISPLAYON);
    display.clearDisplay();
    display.setCursor(0, 8);
    display.println("Starting...");
    display.println();
    display.println("Hold B+C");
    display.println("for setup");
    display.display();

    bool bothPressed =
      (digitalRead(BUTTON_B) == LOW) &&
      (digitalRead(BUTTON_C) == LOW);

    if (bothPressed) {
      if (bothPressedStart == 0) {
        bothPressedStart = millis();
      }

      if (millis() - bothPressedStart >= MENU_HOLD_MS) {
        waitForMenuButtonsRelease();
        return true;
      }
    } else {
      bothPressedStart = 0;
    }

    delay(20);
  }

  return false;
}


// -----------------------------------------------------------------------------
// Draw the RTC adjustment page.
// minuteOffset changes only whole minutes; seconds continue to run normally.
// -----------------------------------------------------------------------------
void drawRtcTimePage(int32_t minuteOffset) {
  DateTime liveNow = rtc.now();

  int64_t shiftedUnix =
    (int64_t)liveNow.unixtime() + ((int64_t)minuteOffset * 60LL);

  DateTime shownTime((uint32_t)shiftedUnix);

  display.oled_command(SH110X_DISPLAYON);
  display.clearDisplay();
  display.setCursor(0, 0);

  display.println("RTC time");
  display.println();

  // YYYY-MM-DD
  display.print(shownTime.year());
  display.print('-');
  menuPrint2Digits(shownTime.month());
  display.print('-');
  menuPrint2Digits(shownTime.day());
  display.println();

  // hh:mm:ss
  menuPrint2Digits(shownTime.hour());
  display.print(':');
  menuPrint2Digits(shownTime.minute());
  display.print(':');
  menuPrint2Digits(shownTime.second());
  display.println();
  display.println();

  display.println("B +1 min");
  display.println("C -1 min");
  display.display();
}


// -----------------------------------------------------------------------------
// RTC time adjustment.
//
// B (pin 6): +1 minute
// C (pin 5): -1 minute
//
// Every button action resets the 3-second inactivity timer.
// After 3 seconds with no button action, the accumulated minute adjustment is
// written to the RTC and this function returns automatically.
//
// There is NO special B+C action here.
// -----------------------------------------------------------------------------
void set_rtc_time() {
  int32_t minuteOffset = 0;
  unsigned long functiontime = millis();

  while (millis() - functiontime < RTC_INACTIVITY_MS) {
    drawRtcTimePage(minuteOffset);

    bool b = (digitalRead(BUTTON_B) == LOW);
    bool c = (digitalRead(BUTTON_C) == LOW);

    // Use two independent checks. There is deliberately no "both" command.
    // If both are pressed at exactly the same time, +1 and -1 cancel out.
    if (b) {
      minuteOffset++;
      functiontime = millis();
      delay(150);
    }

    if (c) {
      minuteOffset--;
      functiontime = millis();
      delay(150);
    }

    if (!b && !c) {
      delay(20);
    }
  }

  // Apply the adjustment only after the page times out.
  // Adding whole multiples of 60 seconds preserves the seconds field while
  // correctly rolling minute/hour/day/month/year forward or backward.
  if (minuteOffset != 0) {
    DateTime liveNow = rtc.now();

    int64_t adjustedUnix =
      (int64_t)liveNow.unixtime() + ((int64_t)minuteOffset * 60LL);

    rtc.adjust(DateTime((uint32_t)adjustedUnix));
  }
}


// -----------------------------------------------------------------------------
// Settings sequence.
// No selectable main menu is needed. Each page advances automatically after
// its inactivity timeout.
//
// Device and Servo use your existing functions from:
//   SetDeviceNum.ino
//   SetServoIteration.ino
// Those functions already save automatically after 3 seconds of inactivity.
// -----------------------------------------------------------------------------
void runSettingsMenu() {
  set_device_num();
  set_servo_iternum();
  set_rtc_time();
}