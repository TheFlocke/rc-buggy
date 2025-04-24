#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>
TwoWire I2C = TwoWire(0);

Adafruit_PWMServoDriver myServos = Adafruit_PWMServoDriver(0x40, (TwoWire&)I2C);

#define SERVOMIN  150
#define SERVOMAX  600

uint8_t servonum = 0;
uint8_t numberOfServos = 1;


void setup() {
    I2C.begin(14, 15, 100000);
    myServos.begin();
    myServos.setPWMFreq(60);
    delay(10);
}

void loop() {
    for (uint16_t pulselen = SERVOMIN; pulselen < SERVOMAX; pulselen++){
        myServos.setPWM(servonum, 0, pulselen);
    }
    delay(10);

    for (uint16_t pulselen = SERVOMAX; pulselen > SERVOMIN; pulselen--){
        myServos.setPWM(servonum, 0, pulselen);
    }
    delay(500);

    servonum ++;
    if (servonum > numberOfServos-1) servonum = 0;
}