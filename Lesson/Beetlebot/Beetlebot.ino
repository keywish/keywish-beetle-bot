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

#define IA_THRESHOLD 40
#define UL_LIMIT_MIN 50
#define UL_LIMIT_MID 40
#define UL_LIMIT_MAX 2000

ProtocolParser *mProtocol = new ProtocolParser();
Hummerbot beetle(mProtocol, INPUT2_PIN, INPUT1_PIN, INPUT3_PIN, INPUT4_PIN);
byte Ps2xStatus, Ps2xType;
ST_PROTOCOL SendData;

void setup()
{
    Serial.begin(9600);
    beetle.init();
    beetle.SetControlMode(E_BLUETOOTH_CONTROL);
    beetle.SetIrPin(BE_IR_PIN);
    beetle.SetInfraredAvoidancePin(BE_INFRARED_AVOIDANCE_LEFT_PIN, BE_INFRARED_AVOIDANCE_RIGHT_PIN);
    beetle.SetUltrasonicPin(BE_TRIGPIN, BE_ECHOPIN, BE_SERVOPIN);
    beetle.SetInfraredTracingPin(BE_INFRARED_TRACING_PIN1, BE_INFRARED_TRACING_PIN2, BE_INFRARED_TRACING_PIN3);
    beetle.SetPs2xPin(BE_PS2X_CLK, BE_PS2X_CMD, BE_PS2X_ATT, BE_PS2X_DAT);
    beetle.SetSpeed(100);
    Ps2xType = beetle.mPs2x->readType();
    beetle.mUltrasonic->SetServoBaseDegree(90);
    beetle.mUltrasonic->SetServoDegree(90);
}
//=========================== Ultrasonic_Infrared =====
void HandleUltrasonicInfraredAvoidance()
{
    uint16_t RightValue,LeftValue;
    uint16_t UlFrontDistance,UlLeftDistance,UlRightDistance;
    RightValue = beetle.mInfraredAvoidance->GetInfraredAvoidanceRightValue();
    LeftValue = beetle.mInfraredAvoidance->GetInfraredAvoidanceLeftValue();
    UlFrontDistance =  beetle.mUltrasonic->GetUltrasonicFrontDistance();
    if(((RightValue > IA_THRESHOLD) && (LeftValue > IA_THRESHOLD)) && ((UlFrontDistance > UL_LIMIT_MID) && (UlFrontDistance < UL_LIMIT_MAX)))
    {
        beetle.SetSpeed(100);
        beetle.GoForward();
      }
    else if((RightValue > IA_THRESHOLD) && (LeftValue < IA_THRESHOLD))
    {
        beetle.SetSpeed(80);
        beetle.TurnRight();
        delay(200);
      }
    else if((RightValue < IA_THRESHOLD) && (LeftValue > IA_THRESHOLD))
    {
        beetle.SetSpeed(80);
        beetle.TurnLeft();
        delay(200);
      }
    else if((RightValue < IA_THRESHOLD) && (LeftValue < IA_THRESHOLD))
    {
        beetle.SetSpeed(80);
        beetle.Drive(0);
        delay(150);
      }
    else if(((RightValue > IA_THRESHOLD) && (LeftValue > IA_THRESHOLD)) && ((UlFrontDistance < UL_LIMIT_MID) || (UlFrontDistance > UL_LIMIT_MAX)))
    {
        beetle.KeepStop();
        UlRightDistance = beetle.mUltrasonic->GetUltrasonicRightDistance();
        UlLeftDistance = beetle.mUltrasonic->GetUltrasonicLeftDistance();
        if((UlRightDistance > UL_LIMIT_MIN) && (UlRightDistance < UL_LIMIT_MAX) && (UlRightDistance > UlLeftDistance))
        {
            beetle.SetSpeed(80);
            beetle.TurnRight();
            delay(200);
          }
        else if((UlLeftDistance > UL_LIMIT_MIN) && (UlLeftDistance < UL_LIMIT_MAX) && (UlLeftDistance > UlRightDistance))
        {
            beetle.SetSpeed(80);
            beetle.TurnLeft();
            delay(200);
          }
        else if((UlRightDistance < UL_LIMIT_MIN) && (UlLeftDistance < UL_LIMIT_MIN) )
        {
            beetle.SetSpeed(80);
            beetle.Drive(0);
            delay(300);
          }
      }
}

void HandleUltrasonicAvoidance()
{
    uint16_t UlFrontDistance,UlLeftDistance,UlRightDistance;
    UlFrontDistance =  beetle.mUltrasonic->GetUltrasonicFrontDistance();
    if ((UlFrontDistance > UL_LIMIT_MID) && (UlFrontDistance < UL_LIMIT_MAX))
    {
        beetle.SetSpeed(100);
        beetle.GoForward();
    }
    else if ((UlFrontDistance < UL_LIMIT_MIN) || (UlFrontDistance > UL_LIMIT_MAX))
    {
        beetle.KeepStop();
        UlRightDistance = beetle.mUltrasonic->GetUltrasonicRightDistance();
        UlLeftDistance = beetle.mUltrasonic->GetUltrasonicLeftDistance();
        if((UlRightDistance > UL_LIMIT_MIN) && (UlRightDistance < UL_LIMIT_MAX) && (UlRightDistance > UlLeftDistance))
        {
            beetle.SetSpeed(80);
            beetle.TurnRight();
            delay(200);
        }
        else if((UlLeftDistance > UL_LIMIT_MIN) && (UlLeftDistance < UL_LIMIT_MAX) && (UlLeftDistance > UlRightDistance))
        {
            beetle.SetSpeed(80);
            beetle.TurnLeft();
            delay(200);
        }
        else if((UlRightDistance < UL_LIMIT_MIN) && (UlLeftDistance < UL_LIMIT_MIN) )
        {
            beetle.SetSpeed(80);
            beetle.Drive(0);
            delay(200);
        }
    }
}
//=============================Infrared tracking
void HandleInfraredTracing(void)
{
      switch (beetle.mInfraredTracing->getValue()) {
        case IT_ALL_BLACK:
        case IT_ALL_WHITE:
          beetle.KeepStop();
          break;
        case IT_CENTER:
          beetle.SetSpeed(80);
          beetle.GoForward();
          break;
        case IT_RIGHT1:
          beetle.SetSpeed(60);
          beetle.Drive(160);
          break;
        case IT_LEFT1:
          beetle.SetSpeed(60);
          beetle.Drive(30);
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
      }
    else if((RightValue > IA_THRESHOLD)&&(LeftValue < IA_THRESHOLD))
    {
        beetle.SetSpeed(70);
        beetle.TurnRight();
        delay(200);
    }
    else if((RightValue < IA_THRESHOLD)&&(LeftValue > IA_THRESHOLD))
    {
        beetle.SetSpeed(70);
        beetle.TurnLeft();
        delay(200);
    }
    else if((RightValue < IA_THRESHOLD)&&(LeftValue < IA_THRESHOLD))
    {
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
    }
      else {
        beetle.KeepStop();
      }
  delay(50);
}

void SendTracingSignal(){
    unsigned int TracingSignal = beetle.mInfraredTracing->getValue();
    SendData.start_code = 0xAA;
    SendData.type = 0x01;
    SendData.addr = 0x01;
    SendData.function = E_INFRARED_TRACKING;
    SendData.data = (byte *)&TracingSignal;
    SendData.len = 7;
    SendData.end_code = 0x55;
    mProtocol->SendPackage(&SendData, 1);
}

void SendInfraredData(){
    unsigned int RightValue = beetle.mInfraredAvoidance->GetInfraredAvoidanceRightValue();
    unsigned int LeftValue = beetle.mInfraredAvoidance->GetInfraredAvoidanceLeftValue();
    byte buffer[2];
    SendData.start_code = 0xAA;
    SendData.type = 0x01;
    SendData.addr = 0x01;
    SendData.function = E_INFRARED_AVOIDANCE_MODE;
    buffer[0] = LeftValue & 0xFF;
    buffer[1] = RightValue & 0xFF;
    SendData.data = buffer;
    SendData.len = 8;
    SendData.end_code = 0x55;
    mProtocol->SendPackage(&SendData, 2);
}

void SendUltrasonicData(){
    unsigned int UlFrontDistance =  beetle.mUltrasonic->GetUltrasonicFrontDistance();
    SendData.start_code = 0xAA;
    SendData.type = 0x01;
    SendData.addr = 0x01;
    SendData.function = E_ULTRASONIC_AVOIDANCE;
    SendData.data = (byte *)&UlFrontDistance;
    SendData.len = 7;
    SendData.end_code = 0x55;
    mProtocol->SendPackage(&SendData, 1);
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
            }
            else {
                if (beetle.GetStatus() != E_STOP ) {
                    beetle.KeepStop();
                    }
                 }
            break;
        case E_INFRARED_TRACKING_MODE:
            DEBUG_LOG(DEBUG_LEVEL_INFO, "E_INFRARED_TRACKING \n");
            HandleInfraredTracing();
            SendTracingSignal();
            break;
        case E_INFRARED_AVOIDANCE:
            DEBUG_LOG(DEBUG_LEVEL_INFO, "E_INFRARED_AVOIDANCE \n");
            HandleInfraredAvoidance();
            break;
        case E_ULTRASONIC_AVOIDANCE:
            DEBUG_LOG(DEBUG_LEVEL_INFO, "E_ULTRASONIC_AVOIDANCE \n");
            HandleUltrasonicAvoidance();
            break;
		    case E_ULTRASONIC_INFRARED_AVOIDANCE:
            DEBUG_LOG(DEBUG_LEVEL_INFO, "E_ULTRASONIC_INFRARED_AVOIDANCE \n");
            HandleUltrasonicInfraredAvoidance();
            SendInfraredData();
            SendUltrasonicData();
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
