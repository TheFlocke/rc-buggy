#include "Arduino.h"
#include "../lib/servo.h"


Adafruit_PWMServoDriver servo = Adafruit_PWMServoDriver(0x40,(TwoWire&)I2CBUS);

#define SERVOMIN 150
#define SERVOMAX 600
#define SERVO_FREQ 50


void Servo::set(int pin, int angle){
    int duty=(map(angle,0,180,SERVOMIN,SERVOMAX));

    servo.setPWM(pin, 0, duty);
}

void Servo::setup() {
    servo.begin();
    servo.setPWMFreq(SERVO_FREQ);
}

