/***********************************************************************
         __                                                          _
        / /        _____  __    __  _          _   (_)   ________   | |
       / /____   / _____) \ \  / / | |   __   | |  | |  (  ______)  | |_____
      / / ___/  | |_____   \ \/ /  | |  /  \  | |  | |  | |______   |  ___  |
     / /\ \     | |_____|   \  /   | | / /\ \ | |  | |  (_______ )  | |   | |
    / /  \ \__  | |_____    / /    | |/ /  \ \| |  | |   ______| |  | |   | |
   /_/    \___\  \______)  /_/      \__/    \__/   |_|  (________)  |_|   |_|


   Keywish Tech firmware

   Copyright (C) 2015-2020

   This program is free software: you can redistribute it and/or modify it
   under the terms of the GNU General Public License as published by the
   Free Software Foundation, in version 3.
   learn more you can see <http://www.gnu.org/licenses/>.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
   or FITNESS FOR A PARTICULAR PURPOSE.


   [Title]
   [Diagram]
*/
#include "Beetlebot.h"
#include "ProtocolParser.h"
#include "KeyMap.h"
#include "debug.h"

#define INPUT2_PIN 10 // PWMB
#define INPUT1_PIN 6  // DIRB  ---  right
#define INPUT4_PIN 9  // PWMA
#define INPUT3_PIN 5  // DIRA  ---  left

#define INFRARED_TRACING_PIN1 A0
#define INFRARED_TRACING_PIN2 A1
#define INFRARED_TRACING_PIN3 A2


ProtocolParser *mProtocol = new ProtocolParser();
Beetlebot beetle(mProtocol, INPUT2_PIN, INPUT1_PIN, INPUT3_PIN, INPUT4_PIN);


void setup()
{
    Serial.begin(9600);
    beetle.init();
    beetle.SetSpeed(0);
    beetle.SetControlMode(E_INFRARED_TRACKING_MODE);
    beetle.SetInfraredTracingPin(BE_INFRARED_TRACING_PIN1, BE_INFRARED_TRACING_PIN2, BE_INFRARED_TRACING_PIN3);
    Serial.println("Get last update from https://github.com/keywish/keywish-beetle-bot");
}
//=============================Infrared tracking
void HandleInfraredTracing(void)
{
    static byte old;
    switch (beetle.mInfraredTracing->getValue()) {
        case IT_ALL_BLACK:
          beetle.KeepStop();
          break;
        case IT_ALL_WHITE:
          if(old == IT_RIGHT1){
          while(beetle.mInfraredTracing->getValue()==IT_ALL_WHITE){
              beetle.SetSpeed(40);
              beetle.Drive(30);}
              old = 0;
              break;
            }
          if(old == IT_LEFT1){
            while(beetle.mInfraredTracing->getValue()==IT_ALL_WHITE){
              beetle.SetSpeed(40);
              beetle.Drive(150);}
              old = 0;
              break;
          }
          break;
        case IT_CENTER:
          beetle.SetSpeed(60);
          beetle.GoForward();
          break;
        case IT_RIGHT1:
          beetle.SetSpeed(40);
          beetle.Drive(30);
          old = IT_RIGHT1;
          break;
        case IT_RIGHT2:
          beetle.SetSpeed(50);
          beetle.Drive(70);
          old = 0;
          break;
        case IT_LEFT1:
          beetle.SetSpeed(40);
          beetle.Drive(150);
          old = IT_LEFT1;
          break;
        case IT_LEFT2:
          beetle.SetSpeed(50);
          beetle.Drive(110);
          old = 0;
          break;
    }
}

void loop()
{
    mProtocol->RecevData();
    switch(beetle.GetControlMode())
    {
        case E_INFRARED_TRACKING_MODE:
            DEBUG_LOG(DEBUG_LEVEL_INFO, "E_INFRARED_TRACKING \n");
            HandleInfraredTracing();
            break;
        default:
            break;
    }
}
