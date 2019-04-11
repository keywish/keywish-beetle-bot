#include "Ultrasonic.h"

static uint16_t history = 0;

 Ultrasonic::Ultrasonic(byte trig_pin, byte echo_pin, byte servo_pin)
{
    TrigPin = trig_pin;
    EchoPin = echo_pin;
    ServoPin = servo_pin;
    pinMode(EchoPin, INPUT);
    pinMode(TrigPin, OUTPUT);
    pinMode(ServoPin,OUTPUT);
}

uint16_t Ultrasonic::GetUltrasonicFrontDistance()
{
    static byte count = 0;
    do {
        digitalWrite(TrigPin, LOW);
        delayMicroseconds(2);
        digitalWrite(TrigPin, HIGH);
        delayMicroseconds(10);
        digitalWrite(TrigPin, LOW);
        FrontDistance = pulseIn(EchoPin, HIGH) / 58.00;
        count++;
    } while ((FrontDistance > UL_LIMIT_MAX) && (count > 3));
    count = 0;
   /* if (history != 0 && FrontDistance - history > 80 ) {
        FrontDistance = history;
        history = 0;
    } else {
      history = FrontDistance;
    } */
    return FrontDistance;
}

uint16_t Ultrasonic::GetUltrasonicLeftDistance()
{
    SetServoDegree(170);
    history = 0;
    LeftDistance = GetUltrasonicFrontDistance();
    SetServoDegree(90);
    return LeftDistance;
}

uint16_t Ultrasonic::GetUltrasonicRightDistance()
{
    SetServoDegree(15);
    history = 0;
    RightDistance = GetUltrasonicFrontDistance();
    SetServoDegree(90);
    return RightDistance;
}

void Ultrasonic::SetServoBaseDegree(uint8_t base)
{
    ServoBaseDegree = base;
}

void Ultrasonic::SetServoDegree(int Angle)
{
  int servo_degree;
  if (ServoDegree == Angle)
  return;
  ServoDegree = Angle;
	if (Angle > 180) {
		servo_degree = 180;
	} else if( Angle < 10) {
        servo_degree = 10;
    } else if (Angle == 90) {
		servo_degree = ServoBaseDegree;
	} else {
		servo_degree = 90 - ServoBaseDegree + Angle;   // 180-degree-diff
	}

	for (int i = 0; i < 80; i++) {
		float pulsewidth = (servo_degree * 11) + 350;
		digitalWrite(ServoPin, HIGH);   //Set the servo interface level to high
		delayMicroseconds(pulsewidth);  //The number of microseconds of the delay pulse width value
		digitalWrite(ServoPin, LOW);    //Set the servo interface level to low
		delayMicroseconds(20000 - pulsewidth);
	}
	delay(250);
}
