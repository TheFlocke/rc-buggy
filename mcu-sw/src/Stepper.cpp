#include <Arduino.h>
#include "../lib/Stepper.h"

// No UART communcation because it is very unstable and all needed settings can be set by pullup/down resistors

// Your motor parts:
#define FULL_STEPS 200.0   // 1.8 degrees per whole step
#define MICROSTEPS 8     // 8, 16, 32, 64, 128, 256
#define GEAR_REDUCTION 3.0       // Gearbox translation

// Don't edit these:

#define REV_STEPS            (FULL_STEPS * MICROSTEPS * GEAR_REDUCTION) // How many Steps are needed for one Revolution

Stepper stepper;

// setting up FactAccel
FastAccelStepperEngine engine = FastAccelStepperEngine();
// putting them in a array for more efficiency
FastAccelStepper *steppers[2] = {nullptr, nullptr};


void Stepper::setup(int STEP0, int STEP1, int DIR0, int DIR1) {
    // init FastAccelStep
    engine.init();
    int stepPins[2] = {STEP0, STEP1};
    int dirPins[2] = {DIR0, DIR1};
    // Setting up Driver
    for (int i = 0; i < 2; i++) {
        // setting up Fastaccelstepper for both Steppers
        steppers[i] = engine.stepperConnectToPin(stepPins[i], 2);
        if (steppers[i]) {
            steppers[i]->setDirectionPin(dirPins[i]);
            steppers[i]->setAcceleration(5000);
        } else {
        }
    }
}


void Stepper::set(int stepperindex, int speed) {
    // setting limits
    speed = constrain(speed, -255, 255);
    if (!steppers[stepperindex]) {
        return;
    }

    if (speed == 0) {
        steppers[stepperindex]->stopMove();
        return;
    }

    bool direction = speed > 0;
    uint32_t mapped_speed = map(abs(speed), 0, 255, 0, REV_STEPS);

    steppers[stepperindex]->setSpeedInHz(mapped_speed);
    if (direction) {
        steppers[stepperindex]->runForward();
    } else {
        steppers[stepperindex]->runBackward();
    }
}