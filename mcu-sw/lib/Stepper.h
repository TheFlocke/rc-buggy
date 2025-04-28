//
// Created by Philipp Winterberg on 25.04.25.
//

#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>
#include <TMCStepper.h>
#include <FastAccelStepper.h>




class Stepper {
    int _PSTEP1_Dir = 4;
    int _PSTEP1_STEP = 6;
    int _PSTEP2_Dir = 5;
    int _PSTEP2_STEP = 7;

    FastAccelStepper *stepper0 = NULL; // Pointers of all Steppers who are connected
    FastAccelStepper *stepper1 = NULL;


public:
    void setup();

    void stepper_0(int speed);

    void stepper_1(int speed);

};


extern Stepper stepper;
#endif //STEPPER_H
