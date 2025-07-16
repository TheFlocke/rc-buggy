//
// Created by Philipp Winterberg on 25.04.25.
//

#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>
#include <TMCStepper.h>
#include <FastAccelStepper.h>


class Stepper {
    // Array init for 2 steppers
    FastAccelStepper *steppers[2] = {nullptr, nullptr};

public:
    void setup(int STEP0, int STEP1, int DIR0, int DIR1);

    void set(int stepperIndex, int speed);
};


extern Stepper stepper;
#endif //STEPPER_H
