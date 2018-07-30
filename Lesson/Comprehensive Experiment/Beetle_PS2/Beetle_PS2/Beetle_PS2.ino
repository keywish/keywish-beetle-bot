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

#define PS2X_CLK 11
#define PS2X_CMD 7
#define PS2X_CS  8
#define PS2X_DAT 4

ProtocolParser *mProtocol = new ProtocolParser();
Beetlebot beetle(mProtocol, INPUT2_PIN, INPUT1_PIN, INPUT3_PIN, INPUT4_PIN);
byte Ps2xStatus, Ps2xType;

void setup()
{
    Serial.begin(9600);
    Serial.println("Get last update from https://github.com/keywish/keywish-beetle-bot");
    beetle.init();
    beetle.SetControlMode(E_PS2_REMOTE_CONTROL);
    beetle.SetPs2xPin(BE_PS2X_CLK, BE_PS2X_CMD, BE_PS2X_ATT, BE_PS2X_DAT);
    beetle.SetSpeed(0);
    Ps2xType = beetle.mPs2x->readType();
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
    }
      else {
        beetle.KeepStop();
      }
  delay(50);
}

void loop()
{
    mProtocol->RecevData();
    switch(beetle.GetControlMode())
    {
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
