//
// Created by Philipp Winterberg on 25.04.25.
//

#ifndef STEPPER_H
#define STEPPER_H

#include <Arduino.h>
#include <TMCStepper.h>
#include <FastAccelStepper.h>




class Stepper {
    int _PSTEP0_Dir = -1;
    int _PSTEP0_STEP = -1;
    int _PSTEP1_Dir = -1;
    int _PSTEP1_STEP = -1;
    int _RX_PIN = -1;
    int _TX_PIN = -1;

    FastAccelStepper *stepper0 = NULL; // Pointers of all Steppers who are connected
    FastAccelStepper *stepper1 = NULL;


public:
    void setup(int STEP0, int STEP1, int DIR0, int DIR1, int RX_PIN, int TX_PIN);

    void stepper_0(int speed);

    void stepper_1(int speed);

};


extern Stepper stepper;
#endif //STEPPER_H
