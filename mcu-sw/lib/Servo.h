#ifndef ServoStep_h
#define ServoStep_h

#include <../lib/Servo.h>
#include "../lib/I2C.h"
#include <Adafruit_PWMServoDriver.h>


class Servo {

public:
    static void set(int pin, int angle);

    static void setup();
};

extern Servo servo;
#endif
