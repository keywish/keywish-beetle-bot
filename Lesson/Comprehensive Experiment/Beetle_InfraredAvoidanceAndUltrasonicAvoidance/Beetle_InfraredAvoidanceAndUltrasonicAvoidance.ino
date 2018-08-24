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
#define INFRARED_AVOIDANCE_LEFT_PIN A3
#define INFRARED_AVOIDANCE_RIGHT_PIN A4

ProtocolParser *mProtocol = new ProtocolParser();
Beetlebot beetle(mProtocol, INPUT2_PIN, INPUT1_PIN, INPUT3_PIN, INPUT4_PIN);

void setup()
{
    Serial.begin(9600);
    beetle.init();
    beetle.SetSpeed(0);
    beetle.SetControlMode(E_ULTRASONIC_INFRARED_AVOIDANCE);
    beetle.SetUltrasonicPin(BE_TRIGPIN, BE_ECHOPIN, BE_SERVOPIN);
    beetle.SetInfraredAvoidancePin(BE_INFRARED_AVOIDANCE_LEFT_PIN, BE_INFRARED_AVOIDANCE_RIGHT_PIN);
    beetle.mUltrasonic->SetServoBaseDegree(90);
    beetle.mUltrasonic->SetServoDegree(90);
    Serial.println("Get last update from https://github.com/keywish/keywish-beetle-bot");
}

//=========================== Ultrasonic_Infrared =====
void HandleUltrasonicInfraredAvoidance()
{
    uint16_t RightValue,LeftValue;
    uint16_t UlFrontDistance,UlLeftDistance,UlRightDistance;
    RightValue = beetle.mInfraredAvoidance->GetInfraredAvoidanceRightValue();
    LeftValue = beetle.mInfraredAvoidance->GetInfraredAvoidanceLeftValue();
    DEBUG_LOG(DEBUG_LEVEL_INFO, "RightValue = %d \n\r", RightValue);
    DEBUG_LOG(DEBUG_LEVEL_INFO, "LeftValue = %d \n\r", LeftValue);
    UlFrontDistance =  beetle.mUltrasonic->GetUltrasonicFrontDistance();
    if ((RightValue >= IA_THRESHOLD) && (LeftValue <= IA_THRESHOLD)) {
        beetle.SetSpeed(100);
        beetle.Drive(45);
    } else if ((RightValue < IA_THRESHOLD) && (LeftValue > IA_THRESHOLD)) {
        beetle.SetSpeed(100);
        beetle.Drive(135);
    } else {
        beetle.SetSpeed(60);
        beetle.GoForward();
    }
    DEBUG_LOG(DEBUG_LEVEL_INFO, "UlFrontDistance = %d \n\r", UlFrontDistance);
    if (UlFrontDistance < UL_LIMIT_MID) {
        beetle.KeepStop();
        if (UlFrontDistance <= UL_LIMIT_MIN || RightValue < IA_THRESHOLD || LeftValue < IA_THRESHOLD) {
            beetle.SetSpeed(60);
            beetle.GoBack();
            delay(300);
            beetle.KeepStop();
        }
       // if ((RightValue > IA_THRESHOLD) && (LeftValue > IA_THRESHOLD)) {
            beetle.KeepStop();
            UlRightDistance = beetle.mUltrasonic->GetUltrasonicRightDistance();
            UlLeftDistance = beetle.mUltrasonic->GetUltrasonicLeftDistance();
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
       // }
    }
}

void loop()
{
    mProtocol->RecevData();
    switch(beetle.GetControlMode())
    {
	    case E_ULTRASONIC_INFRARED_AVOIDANCE:
            // DEBUG_LOG(DEBUG_LEVEL_INFO, "E_ULTRASONIC_INFRARED_AVOIDANCE \n");
            HandleUltrasonicInfraredAvoidance();
            break;
        default:
            break;
    }
}
