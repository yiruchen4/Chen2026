/********************************************************
  Social Door v1.0
  Written by Lex Kravitz and Yiru Chen
  March 2024

  Events:
  1) Pulse detected
  2) Start door opening
  3) Door opened
  4) Start door closing
  5) Door closed

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2024 Lex Kravitz
  ********************************************************/

#include "a_Header.h"  //See "a_Header.h" for #defines and other constants

void setup() {
  StartUpCommands();
}

void loop() {
  // When input pulse is received
  if (digitalRead(12) == HIGH) {
    button = 0;
    open_num++;
    new_trial = millis();
    // Decide ONCE whether this trial is an opto trial
    opto_trial = check_opto_trial();
    open_door();
  }


  // When button is pushed
  if (digitalRead(6) == LOW) {
    opto_trial = check_opto_trial();
    open_door_button();
  }

  // After the screen turns on after single press on Button C,
  // quickly press Button C for 3+ times will display opto windows and emit 1s 20Hz opto light
  if (digitalRead(5) == LOW) {
    screen_time = millis();
    int origin_door = door;
    door = -99;
    read_sensors();
    update_display();
    delay(100);
    int press = 0;
    while (millis() - screen_time < 1500) {
      if (digitalRead(5) == LOW) {
        press++;
      }
    }
    if (press >= 3) {
      opto_check();
    }
    door = origin_door;
  }


  if (millis() - screen_time > 20000) {
    //turn off the screen
    display.oled_command(SH110X_DISPLAYOFF);
    digitalWrite(13, LOW);
    LowPower.sleep(1000);
  }
}

bool time_in_window(
  int currentTime,
  int startTime,
  int endTime) {

  // Equal start/end = disabled
  if (startTime == endTime) {
    return false;
  }

  // Normal window
  // Example: 21:00 -> 23:00
  if (startTime < endTime) {

    return (
      currentTime >= startTime && currentTime < endTime);
  }

  // Window crosses midnight
  // Example: 23:00 -> 01:00
  return (
    currentTime >= startTime || currentTime < endTime);
}

bool check_opto_trial() {

  DateTime now = rtc.now();

  int currentTime =
    now.hour() * 60 + now.minute();


  // Check each available opto window
  for (int i = 0; i < NUM_OPTO_WINDOWS; i++) {

    if (
      time_in_window(
        currentTime,
        optoWindows[i].startTime,
        optoWindows[i].endTime)) {

      return true;
    }
  }


  // Current trial is not inside any opto window
  return false;
}

void display_opto_windows() {

  display.clearDisplay();
  display.setCursor(1, 0);

  display.println("Opto Windows");

  for (int i = 0; i < NUM_OPTO_WINDOWS; i++) {

    // Start time
    display.print(i + 1);
    display.print(" ");
    display_opto_time(optoWindows[i].startTime);
    display.println(" -");

    // End time
    display.print("  ");
    display_opto_time(optoWindows[i].endTime);
    display.println();
  }

  display.display();
}

void opto_check() {
  opto = 1;
  door = -88;
  read_sensors();

  // Show configured opto windows
  display_opto_windows();

  for (int i = 0; i < 20; i++) {
    digitalWrite(A2, HIGH);
    digitalWrite(A1, HIGH);
    delay(10);

    digitalWrite(A2, LOW);
    digitalWrite(A1, LOW);
    delay(40);
  }
}