#ifndef ServoStep_h
#define ServoStep_h

#include <../lib/Servo.h>
#include "../lib/i2c_bus.h"
#include <Adafruit_PWMServoDriver.h>


class Servo {

public:
    void set(int pin, int angle);

    void setup();
};

extern Servo servo;
#endif
