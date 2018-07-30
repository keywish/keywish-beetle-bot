/***********************************************************************
 *       __                                                          _
 *      / /        _____  __    __  _          _   (_)   ________   | |
 *     / /____   / _____) \ \  / / | |   __   | |  | |  (  ______)  | |_____
 *    / / ___/  | |_____   \ \/ /  | |  /  \  | |  | |  | |______   |  ___  |
 *   / /\ \     | |_____|   \  /   | | / /\ \ | |  | |  (_______ )  | |   | |
 *  / /  \ \__  | |_____    / /    | |/ /  \ \| |  | |   ______| |  | |   | |
 * /_/    \___\  \______)  /_/      \__/    \__/   |_|  (________)  |_|   |_|
 *
 *
 * KeyWay Tech firmware
 *
 * Copyright (C) 2015-2020
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, in version 3.
 * learn more you can see <http://www.gnu.org/licenses/>.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.
 *
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

ProtocolParser *mProtocol = new ProtocolParser();
Beetlebot beetle(mProtocol, INPUT2_PIN, INPUT1_PIN, INPUT3_PIN, INPUT4_PIN);

void setup()
{
    Serial.begin(9600);
    Serial.println("Get last update from https://github.com/keywish/keywish-beetle-bot");
    beetle.init();
    beetle.SetControlMode(E_INFRARED_REMOTE_CONTROL);
    beetle.SetIrPin(BE_IR_PIN);
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
          beetle.GoForward();
          break;
        case IR_KEYCODE_DOWN:
          beetle.GoBack();
          break;
        case IR_KEYCODE_OK:
          beetle.KeepStop();
          break;
        case IR_KEYCODE_LEFT:
          beetle.TurnLeft();
          break;
        case IR_KEYCODE_RIGHT:
          beetle.TurnRight();
          break;
        default:
          break;
    }
}

void loop()
{
    mProtocol->RecevData();
    switch(beetle.GetControlMode())
    {
        case E_INFRARED_REMOTE_CONTROL:
            byte irKeyCode;
            if (irKeyCode = beetle.mIrRecv->getCode()) {
            DEBUG_LOG(DEBUG_LEVEL_INFO, "irKeyCode = %lx \n", irKeyCode);
            HandleInfaredRemote(irKeyCode);
            delay(110);
            }
            else {
                if (beetle.GetStatus() != E_STOP ) {
                    beetle.KeepStop();
                    }
                 }
            break;
        default:
            break;
    }
}
