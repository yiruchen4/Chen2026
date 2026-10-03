/********************************************************
This code allows continous recording of proxmity regardless of door opening or close.
Warning: Battery life will be largely shortened. Please check battery level more frequently.

  Social Door v3.0
  Written by Lex Kravitz and Yiru Chen
  Feburary 2026

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
  read_sensors();

    // When input pulse is received
  if (digitalRead(12) == HIGH) {
    button = 0;
    open_num++;
    event = 1;
    open_door();
  }


  // When button is pushed
  if (digitalRead(6) == LOW) {
    open_num++;
    event = 1;
    open_door_button();
  }

}
