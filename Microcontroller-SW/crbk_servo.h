#ifndef ServoStep_h
#define ServoStep_h

#include "Arduino.h"

class CRBKServo
{
    public:
        CRBKServo(int pin);

    public:
        void up();
        void down();
        void set(int angle);
    private:
        int _pin;

};

#endif