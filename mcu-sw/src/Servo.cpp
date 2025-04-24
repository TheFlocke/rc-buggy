#include "Arduino.h"
#include "../lib/Servo.h"

Servo servo;

Adafruit_PWMServoDriver pwm_servo = Adafruit_PWMServoDriver(0x40,(TwoWire&)I2CBUS);

#define SERVOMIN 150
#define SERVOMAX 600
#define SERVO_FREQ 50


void Servo::set(int pin, int angle){
    int duty=(map(angle,0,180,SERVOMIN,SERVOMAX));

    pwm_servo.setPWM(pin, 0, duty);
}

void Servo::setup() {
    pwm_servo.begin();
    pwm_servo.setPWMFreq(SERVO_FREQ);
}

