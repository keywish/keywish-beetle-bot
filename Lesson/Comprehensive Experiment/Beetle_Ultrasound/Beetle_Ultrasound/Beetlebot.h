#ifndef _BEETLEBOT_H_
#define _BEETLEBOT_H_
#include <stdint.h>
#include "Arduino.h"
#include "SmartCar.h"
#include "IRremote.h"
#include "PS2X_lib.h"  //for v1.6
#include "InfraredAvoidance.h"
#include "InfraredTracing.h"
#include "ProtocolParser.h"
#include "Ultrasonic.h"

#define BE_IR_PIN 12
#define BE_INPUT2_PIN 10
#define BE_INPUT1_PIN 6
#define BE_INPUT4_PIN 9
#define BE_INPUT3_PIN 5

#define BE_TRIGPIN 2
#define BE_ECHOPIN 3
#define BE_SERVOPIN 13

#define BE_INFRARED_AVOIDANCE_LEFT_PIN A3
#define BE_INFRARED_AVOIDANCE_RIGHT_PIN A4

#define BE_INFRARED_TRACING_PIN1 A0
#define BE_INFRARED_TRACING_PIN2 A1
#define BE_INFRARED_TRACING_PIN3 A2

#define BE_PS2X_CLK 11
#define BE_PS2X_CMD 7
#define BE_PS2X_ATT 8
#define BE_PS2X_DAT 4

class Beetlebot : public SmartCar {
private :
    uint8_t InPut2PIN, InPut1PIN, PwmaPin, InPut3PIN, InPut4PIN;
    uint8_t IrPin;      // Infrared remoter pin
    uint8_t InfraredTracingPin1, InfraredTracingPin2, InfraredTracingPin3, InfraredTracingPin4, InfraredTracingPin5;    // for Infrared tracing pin
    uint8_t Ps2xClkPin, Ps2xCmdPin, Ps2xAttPin, Ps2xDatPin;    // for Ps2 remoter
    uint8_t InfraredAvoidancePin1,InfraredAvoidancePin2;     //For infrared obstacle avoidance
    uint8_t EchoPin,TrigPin,ServoPin;

    ProtocolParser *mProtocolPackage;

public :
    Beetlebot(ProtocolParser *Package, uint8_t input2 = BE_INPUT2_PIN, uint8_t input1 = BE_INPUT1_PIN, uint8_t input3 = BE_INPUT3_PIN, uint8_t input4 = BE_INPUT4_PIN);
    ~Beetlebot();
    IRremote  *mIrRecv;
    PS2X *mPs2x;
    InfraredTracing *mInfraredTracing;
    InfraredAvoidance *mInfraredAvoidance;
    Ultrasonic *mUltrasonic;

    void GoForward(void);
    void GoBack(void);
    void TurnLeft(void);
    void TurnRight(void);
    void KeepStop(void);
    void Drive(void);
    void Drive(int degree);
    void SetIrPin(uint8_t pin = BE_IR_PIN);
    void SetUltrasonicPin(uint8_t Trig_Pin = BE_TRIGPIN, uint8_t Echo_Pin = BE_ECHOPIN, uint8_t Sevo_Pin = BE_SERVOPIN);
    void SetInfraredAvoidancePin(uint8_t Left_Pin = BE_INFRARED_AVOIDANCE_LEFT_PIN, uint8_t Right_Pin = BE_INFRARED_AVOIDANCE_RIGHT_PIN);
    void SetInfraredTracingPin(uint8_t Pin1 = BE_INFRARED_TRACING_PIN1, uint8_t Pin2 = BE_INFRARED_TRACING_PIN2, uint8_t Pin3 = BE_INFRARED_TRACING_PIN3);
    int SetPs2xPin(uint8_t clk = BE_PS2X_CLK, uint8_t cmd = BE_PS2X_CMD, uint8_t att = BE_PS2X_ATT, uint8_t dat = BE_PS2X_DAT);
    int ResetPs2xPin(void);
    void SendBatteryPackage(byte *battery_value);
    void init(void);
};

#endif  /* _HUMMERBOT_H_ */
