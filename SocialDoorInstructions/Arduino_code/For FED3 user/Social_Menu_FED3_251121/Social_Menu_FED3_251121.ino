/*
  Feeding experimentation device 3 (FED3)

  This script demonstrates how to write a menu system so you can access multiple programs from the FED3 startup screen.  You will scroll through programs by assigning them to "modes".

  In this example, we run four modes:
  // FEDmodes:
  // 0 Nose poke for Social Door + Free feeding
  // 1 Nose poke for Social Door
  // 2 Nose poke for feeding (right side)

  alexxai@wustl.edu
  December, 2020

  This project is released under the terms of the Creative Commons - Attribution - ShareAlike 3.0 license:
  human readable: https://creativecommons.org/licenses/by-sa/3.0/
  legal wording: https://creativecommons.org/licenses/by-sa/3.0/legalcode
  Copyright (c) 2020 Lex Kravitz

*/

#include <FED3.h>        //Include the FED3 library
String sketch = "Menu";  //Unique identifier text for each sketch
FED3 fed3(sketch);       //Start the FED3 object
float minPokeTime = 200;
int reverse_n = 20;
int reward_num = 0;
bool LeftActive = true;

void setup() {
  fed3.FED3Menu = true;  //Activate the menu function at startup
  fed3.begin();          //Setup the FED3 hardware
}

void loop() {
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                    Call fed.run at least once per loop
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  fed3.run();


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 1: Social (Minimum ms poke) + Free Feeding
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (fed3.FEDmode == 0) {
    fed3.sessiontype = "Free_FD";         //The text in "sessiontype" will appear on the screen and in the logfile
    if (digitalRead(LEFT_POKE) == LOW) {  //If left poke is triggered
      fed3.PelletAvailable = false;
      fed3.logLeftPoke();  //Log left poke
      if (fed3.leftInterval < (minPokeTime)) {
        fed3.Click();
      } else {
        fed3.ConditionedStimulus();  //Deliver conditioned stimulus (tone and lights)
        fed3.BNC(500, 1);            //open door
        fed3.Timeout(15);
      }
    }

    if (digitalRead(RIGHT_POKE) == LOW) {  //If right poke is triggered
      fed3.PelletAvailable = false;
      fed3.logRightPoke();
      if (fed3.rightInterval >= (minPokeTime)) {
        fed3.Click();
      }
    }

    if (digitalRead(1) == HIGH) {
      fed3.Timeout(3);  // add on 7/11/24
      fed3.Social_Feed(); // modified function in src folder, allowing the mouse to nose poke when the pellet is not taken.
    }
  }


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     Mode 2: Social only: Minimum ms poke
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

  if (fed3.FEDmode == 1) {
    fed3.sessiontype = "DoorOnly";        //The text in "sessiontype" will appear on the screen and in the logfile
    if (digitalRead(LEFT_POKE) == LOW) {  //If left poke is triggered
      fed3.PelletAvailable = false;
      fed3.logLeftPoke();  //Log left poke
      if (fed3.leftInterval < (minPokeTime)) {
        fed3.Click();
      } else {
        fed3.ConditionedStimulus();  //Deliver conditioned stimulus (tone and lights)
        fed3.BNC(500, 1);            //open door
        fed3.Timeout(15);
      }
    }

    if (digitalRead(RIGHT_POKE) == LOW) {  //If right poke is triggered
      fed3.PelletAvailable = false;
      fed3.logRightPoke();
      if (fed3.rightInterval >= (minPokeTime)) {
        fed3.Click();
      }
    }
  }


  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                            Mode 3: Feeding only: Minimum ms poke + FR1 on the right
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  
  if (fed3.FEDmode == 2) {
    fed3.sessiontype = "FR1_Right";  //The text in "sessiontype" will appear on the screen and in the logfile
    bool RightActive = true;
    if (fed3.Right) {
      fed3.logRightPoke();  //Log right poke
      if (fed3.rightInterval < (minPokeTime)) {
        fed3.Click();
      } else {                       //if fixed ratio is  met
        fed3.ConditionedStimulus();  //deliver conditioned stimulus (tone and lights)
        fed3.Feed();                 //deliver pellet
      }
    }
    if (fed3.Left) {  //If left poke is triggered
      fed3.logLeftPoke();
      fed3.Click();
    }
  }

  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  //                                                                     If a mode greater than 3 is selected
  ////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
  if (fed3.FEDmode > 2) {
    fed3.DisplayNoProgram();
  }
}
