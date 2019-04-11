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

#define IR_PIN 12
#define SERVO_PIN 13
#define ECHO_PIN 3
#define TRIG_PIN 2
#define PS2X_CLK 11
#define PS2X_CMD 7
#define PS2X_CS  8
#define PS2X_DAT 4
#define INFRARED_TRACING_PIN1 A0
#define INFRARED_TRACING_PIN2 A1
#define INFRARED_TRACING_PIN3 A2
#define INFRARED_AVOIDANCE_LEFT_PIN A3
#define INFRARED_AVOIDANCE_RIGHT_PIN A4

ProtocolParser *mProtocol = new ProtocolParser();
Beetlebot beetle(mProtocol, INPUT2_PIN, INPUT1_PIN, INPUT3_PIN, INPUT4_PIN);
byte Ps2xStatus, Ps2xType;

void setup()
{
    Serial.begin(9600);
    beetle.init();
    beetle.SetSpeed(100);
    beetle.SetControlMode(E_BLUETOOTH_CONTROL);
    beetle.SetIrPin(BE_IR_PIN);
    beetle.SetUltrasonicPin(BE_TRIGPIN, BE_ECHOPIN, BE_SERVOPIN);
    beetle.SetInfraredAvoidancePin(BE_INFRARED_AVOIDANCE_LEFT_PIN, BE_INFRARED_AVOIDANCE_RIGHT_PIN);
    beetle.SetInfraredTracingPin(BE_INFRARED_TRACING_PIN1, BE_INFRARED_TRACING_PIN2, BE_INFRARED_TRACING_PIN3);
    beetle.mUltrasonic->SetServoBaseDegree(90);
    beetle.mUltrasonic->SetServoDegree(90);
    beetle.SetPs2xPin(BE_PS2X_CLK, BE_PS2X_CMD, BE_PS2X_ATT, BE_PS2X_DAT);
    Ps2xType = beetle.mPs2x->readType();
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
    DEBUG_LOG(DEBUG_LEVEL_INFO, "UlFrontDistance = %d \n", UlFrontDistance);
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
//========================= bluetooth
void HandleBluetoothRemote()
{
    if (mProtocol->ParserPackage())
    {
        switch(mProtocol->GetRobotControlFun())
        {
            case E_INFO:
                break;
            case E_ROBOT_CONTROL_DIRECTION:
                beetle.Drive(mProtocol->GetRobotDegree());
                break;
            case E_ROBOT_CONTROL_SPEED:
                beetle.SetSpeed(mProtocol->GetRobotSpeed());
                break ;
            case E_CONTROL_MODE:
                beetle.SetControlMode(mProtocol->GetControlMode());
                break;
            case E_VERSION:
                break;
        }
    }
}

void HandleInfaredRemote(byte irKeyCode)
{
    switch ((E_IR_KEYCODE)beetle.mIrRecv->getIrKey(irKeyCode)) {
        case IR_KEYCODE_STAR:
          beetle.SpeedUp(10);
          DEBUG_LOG(DEBUG_LEVEL_INFO, "beetle.Speed = %d \n", beetle.Speed);
          break;
        case IR_KEYCODE_POUND:
          DEBUG_LOG(DEBUG_LEVEL_INFO, " start Degree = %d \n", beetle.Degree);
          beetle.SpeedDown(10);
          break;
        case IR_KEYCODE_UP:
          // beetle.SetSpeed(100);
          beetle.GoForward();
          break;
        case IR_KEYCODE_DOWN:
          // beetle.SetSpeed(100);
          beetle.GoBack();
          break;
        case IR_KEYCODE_OK:
          beetle.KeepStop();
          break;
        case IR_KEYCODE_LEFT:
          // beetle.SetSpeed(60);
          beetle.TurnLeft();
          break;
        case IR_KEYCODE_RIGHT:
          // beetle.SetSpeed(60);
          beetle.TurnRight();
          break;
        default:
          break;
    }
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

void HandlePS2()
{
    static int vibrate = 0;
    byte PSS_X = 0, PSS_Y = 0;
    beetle.mPs2x->read_gamepad(false, vibrate); // read controller and set large motor to spin at 'vibrate' speed
    if (beetle.mPs2x->ButtonDataByte()) {
        if (beetle.mPs2x->Button(PSB_PAD_UP)) {     //will be TRUE as long as button is pressed
          beetle.GoForward();
        }
        if (beetle.mPs2x->Button(PSB_PAD_RIGHT)) {
          beetle.Drive(20);
        }
        if (beetle.mPs2x->Button(PSB_PAD_LEFT)) {
          beetle.Drive(160);
        }
        if (beetle.mPs2x->Button(PSB_PAD_DOWN)) {
          beetle.GoBack();
        }
        vibrate = beetle.mPs2x->Analog(PSAB_CROSS);  //this will set the large motor vibrate speed based on how hard you press the blue (X) button
        if (beetle.mPs2x->Button(PSB_CROSS)) {             //will be TRUE if button was JUST pressed OR released
          beetle.SpeedDown(5);
        }
        if (beetle.mPs2x->Button(PSB_TRIANGLE)) {
          beetle.SpeedUp(5);
        }
        if (beetle.mPs2x->Button(PSB_SQUARE)) {
          beetle.TurnLeft();
        }
        if (beetle.mPs2x->Button(PSB_CIRCLE)) {
          beetle.TurnRight();
        }
     } else {
        beetle.KeepStop();
    }
    delay(50);
}

void loop()
{
    mProtocol->RecevData();
    if (beetle.GetControlMode() !=  E_BLUETOOTH_CONTROL) {
        if (mProtocol->ParserPackage()) {
            if (mProtocol->GetRobotControlFun() == E_CONTROL_MODE) {
            beetle.SetControlMode(mProtocol->GetControlMode());
           }
        }
    }
    switch(beetle.GetControlMode())
    {
        case E_BLUETOOTH_CONTROL:
            HandleBluetoothRemote();
            DEBUG_LOG(DEBUG_LEVEL_INFO, "E_BLUETOOTH_CONTROL \n");
            break;
        case E_INFRARED_REMOTE_CONTROL:
            byte irKeyCode;
            if (irKeyCode = beetle.mIrRecv->getCode()) {
            DEBUG_LOG(DEBUG_LEVEL_INFO, "irKeyCode = %lx \n", irKeyCode);
            HandleInfaredRemote(irKeyCode);
            delay(110);
            } else {
                if (beetle.GetStatus() != E_STOP ) {
                    beetle.KeepStop();
                }
             }
            break;
        case E_INFRARED_TRACKING_MODE:
            DEBUG_LOG(DEBUG_LEVEL_INFO, "E_INFRARED_TRACKING \n");
            HandleInfraredTracing();
            beetle.SendTracingSignal();
            break;
        case E_INFRARED_AVOIDANCE_MODE:
            DEBUG_LOG(DEBUG_LEVEL_INFO, "E_INFRARED_AVOIDANCE \n");
            HandleInfraredAvoidance();
            break;
        case E_ULTRASONIC_AVOIDANCE:
            DEBUG_LOG(DEBUG_LEVEL_INFO, "E_ULTRASONIC_AVOIDANCE \n");
            HandleUltrasonicAvoidance();
            break;
	    case E_ULTRASONIC_INFRARED_AVOIDANCE:
            // DEBUG_LOG(DEBUG_LEVEL_INFO, "E_ULTRASONIC_INFRARED_AVOIDANCE \n");
            HandleUltrasonicInfraredAvoidance();
            beetle.SendInfraredData();
            beetle.SendUltrasonicData();
            break;
        case E_PS2_REMOTE_CONTROL:
            while (Ps2xStatus != 0) { //skip loop if no controller found
                delay(500);
                Ps2xStatus = beetle.ResetPs2xPin();
                Ps2xType = beetle.mPs2x->readType();
                DEBUG_LOG(DEBUG_LEVEL_INFO, "E_PS2_REMOTE_CONTROL \n");
            }
            if (Ps2xType != 2) {
              HandlePS2();
            }
            break;
        default:
            break;
    }
}
