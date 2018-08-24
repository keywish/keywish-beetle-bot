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

#define SERVO_PIN 13
#define ECHO_PIN 3
#define TRIG_PIN 2

ProtocolParser *mProtocol = new ProtocolParser();
Beetlebot beetle(mProtocol, INPUT2_PIN, INPUT1_PIN, INPUT3_PIN, INPUT4_PIN);

void setup()
{
    Serial.begin(9600);
    beetle.init();
    beetle.SetSpeed(0);
    beetle.SetControlMode(E_ULTRASONIC_AVOIDANCE);
    beetle.SetUltrasonicPin(BE_TRIGPIN, BE_ECHOPIN, BE_SERVOPIN);
    beetle.mUltrasonic->SetServoBaseDegree(90);
    beetle.mUltrasonic->SetServoDegree(90);
    Serial.println("Get last update from https://github.com/keywish/keywish-beetle-bot");
}

void HandleUltrasonicAvoidance()
{
    uint16_t UlFrontDistance, UlLeftDistance, UlRightDistance;
    UlFrontDistance =  beetle.mUltrasonic->GetUltrasonicFrontDistance();
    DEBUG_LOG(DEBUG_LEVEL_INFO, "UlFrontDistance = %d \n\r", UlFrontDistance);
    if (UlFrontDistance < UL_LIMIT_MID) {
        beetle.KeepStop();
        if (UlFrontDistance <= UL_LIMIT_MIN) {
            beetle.SetSpeed(60);
            beetle.GoBack();
            delay(300);
            beetle.KeepStop();
        }
        UlRightDistance = beetle.mUltrasonic->GetUltrasonicRightDistance();
        UlLeftDistance = beetle.mUltrasonic->GetUltrasonicLeftDistance();
        DEBUG_LOG(DEBUG_LEVEL_INFO, "UlRightDistance = %d \n\r", UlRightDistance);
        DEBUG_LOG(DEBUG_LEVEL_INFO, "UlLeftDistance = %d \n\r", UlLeftDistance);
        if (UlRightDistance >= UlLeftDistance) {
            beetle.SetSpeed(60);
            beetle.TurnRight();
            delay(300);
        }
        if (UlLeftDistance > UlRightDistance) {
            beetle.SetSpeed(60);
            beetle.TurnLeft();
            delay(300);
        }
        if (UlLeftDistance <= UL_LIMIT_MIN && UlRightDistance <= UL_LIMIT_MIN ) {
            beetle.SetSpeed(80);
            beetle.Drive(0);
            delay(480);
        }
        beetle.KeepStop();
    } else {
        beetle.SetSpeed(60);
        beetle.GoForward();
    }
}

void loop()
{
    mProtocol->RecevData();
    switch(beetle.GetControlMode())
    {
        case E_ULTRASONIC_AVOIDANCE:
            DEBUG_LOG(DEBUG_LEVEL_INFO, "E_ULTRASONIC_AVOIDANCE \n");
            HandleUltrasonicAvoidance();
            break;
        default:
            break;
    }
}
