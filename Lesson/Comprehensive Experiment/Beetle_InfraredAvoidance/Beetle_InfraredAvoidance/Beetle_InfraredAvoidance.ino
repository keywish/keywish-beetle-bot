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

#define INFRARED_AVOIDANCE_LEFT_PIN A3
#define INFRARED_AVOIDANCE_RIGHT_PIN A4

ProtocolParser *mProtocol = new ProtocolParser();
Beetlebot beetle(mProtocol, INPUT2_PIN, INPUT1_PIN, INPUT3_PIN, INPUT4_PIN);


void setup()
{
    Serial.begin(9600);
    beetle.init();
    beetle.SetSpeed(0);
    beetle.SetControlMode(E_INFRARED_AVOIDANCE_MODE);
    beetle.SetInfraredAvoidancePin(BE_INFRARED_AVOIDANCE_LEFT_PIN, BE_INFRARED_AVOIDANCE_RIGHT_PIN);
    Serial.println("Get last update from https://github.com/keywish/keywish-beetle-bot");
}


//====================================InfraredAvoidance
void HandleInfraredAvoidance()
{
    uint16_t RightValue,LeftValue;
    RightValue = beetle.mInfraredAvoidance->GetInfraredAvoidanceRightValue();
    LeftValue = beetle.mInfraredAvoidance->GetInfraredAvoidanceLeftValue();
    if((RightValue > IA_THRESHOLD)&&(LeftValue >IA_THRESHOLD))
    {
        beetle.SetSpeed(80);
        beetle.GoForward();
    } else if((RightValue > IA_THRESHOLD)&&(LeftValue < IA_THRESHOLD)) {
        beetle.Drive(45);
    } else if((RightValue < IA_THRESHOLD)&&(LeftValue > IA_THRESHOLD)) {
        beetle.Drive(135);
    } else if((RightValue < IA_THRESHOLD)&&(LeftValue < IA_THRESHOLD)) {
        beetle.SetSpeed(80);
        beetle.Drive(0);
        delay(200);
    }
}

void loop()
{
    mProtocol->RecevData();
    switch(beetle.GetControlMode())
    {
        case E_INFRARED_AVOIDANCE_MODE:
            DEBUG_LOG(DEBUG_LEVEL_INFO, "E_INFRARED_AVOIDANCE \n");
            HandleInfraredAvoidance();
            break;
        default:
            break;
    }
}
