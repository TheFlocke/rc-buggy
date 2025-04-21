#include <crbk_ble.h>

#include "crbk_servo.h"

const int MOTOR1A=D1;
const int MOTOR1B=D2;
const int MOTOR2A=D3;
const int MOTOR2B=D4;

const int MOTOR3A=D7;
const int MOTOR3B=D8;
const int MOTOR4A=D5;
const int MOTOR4B=D10;

CRBKServo servo(D6); 

const int LED1=D0;
const int LED2=D9; // LOW Active

void setup() {
  pinMode(MOTOR1A, OUTPUT);
  pinMode(MOTOR1B, OUTPUT);
  pinMode(MOTOR2A, OUTPUT);
  pinMode(MOTOR2B, OUTPUT);

  pinMode(MOTOR3A, OUTPUT);
  pinMode(MOTOR3B, OUTPUT);
  pinMode(MOTOR4A, OUTPUT);
  pinMode(MOTOR4B, OUTPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);

  crbkRCCarBLE.setup("CRBK-RCCAR-6");

  servo.set(0);
}

void loop() { 
  crbkRCCarBLE.handle();
  int speed = crbkRCCarBLE.getSpeed();
  int left  = abs(speed);
  int right = abs(speed);
  int direction = crbkRCCarBLE.getDirection();
  if(direction<0) {
    // reduziere Geschwindigkeit nach links
    left -= left*(-direction/90.0);
  } else if(direction>0) {
    // reduziere Geschwindigkeit nach rechts
    right -= right*(direction/90.0);
  }
  if(speed>0) {
    // forward
    pinMode(MOTOR1B, OUTPUT);
    pinMode(MOTOR2B, OUTPUT);

    pinMode(MOTOR3B, OUTPUT);
    pinMode(MOTOR4B, OUTPUT);

    analogWrite(MOTOR1A, left);
    digitalWrite(MOTOR1B, LOW);

    analogWrite(MOTOR2A, right);
    digitalWrite(MOTOR2B, LOW);

    analogWrite(MOTOR3A, left);
    digitalWrite(MOTOR3B, LOW);

    analogWrite(MOTOR4A, right);
    digitalWrite(MOTOR4B, LOW);
  } else if(speed<0) {
    // backward
    pinMode(MOTOR1A, OUTPUT);
    pinMode(MOTOR2A, OUTPUT);

    pinMode(MOTOR3A, OUTPUT);
    pinMode(MOTOR4A, OUTPUT);

    analogWrite(MOTOR1B, left);
    digitalWrite(MOTOR1A, LOW);

    analogWrite(MOTOR2B, right);
    digitalWrite(MOTOR2A, LOW);

    analogWrite(MOTOR3B, left);
    digitalWrite(MOTOR3A, LOW);

    analogWrite(MOTOR4B, right);
    digitalWrite(MOTOR4A, LOW);
  } else {
    // stopp
    pinMode(MOTOR1A, OUTPUT);
    pinMode(MOTOR1B, OUTPUT);
    pinMode(MOTOR2A, OUTPUT);
    pinMode(MOTOR2B, OUTPUT);

    pinMode(MOTOR3A, OUTPUT);
    pinMode(MOTOR3B, OUTPUT);
    pinMode(MOTOR4A, OUTPUT);
    pinMode(MOTOR4B, OUTPUT);

    digitalWrite(MOTOR1A, LOW);
    digitalWrite(MOTOR1B, LOW);

    digitalWrite(MOTOR2A, LOW);
    digitalWrite(MOTOR2B, LOW);

    digitalWrite(MOTOR3A, LOW);
    digitalWrite(MOTOR3B, LOW);

    digitalWrite(MOTOR4A, LOW);
    digitalWrite(MOTOR4B, LOW);
  }

  digitalWrite(LED1, crbkRCCarBLE.getLED1() ? HIGH : LOW);
  digitalWrite(LED2, crbkRCCarBLE.getLED2() ? LOW : HIGH);

  servo.set(crbkRCCarBLE.getDirection());
}
