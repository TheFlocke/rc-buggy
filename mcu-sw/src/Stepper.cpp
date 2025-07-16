#include <Arduino.h>
#include "../lib/Stepper.h"


// Your Driver settings
#define SERIAL_PORT Serial1 // TMC2208/TMC2224 HardwareSerial port
#define DRIVER_ADDRESS_0 0b00 // TMC2209 Driver address according to MS1 and MS2
#define DRIVER_ADDRESS_1 0b01 // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.17f // for 1.2A see online on how to calc https://learn.watterott.com/silentstepstick/faq/#calculator-tmc21xx-and-tmc2209



// Your motor parts:
#define FULL_STEPS 200.0   // 1.8 degrees per whole step
#define MICROSTEPS 8     // 8, 16, 32, 64, 128, 256
#define GEAR_REDUCTION 3.0       // Gearbox translation
#define RMS_CURRENT 1200  // in mA max 2.1A

// Don't edit these:
#define REV_STEPS            (FULL_STEPS * MICROSTEPS * GEAR_REDUCTION) // How many Steps are needed for one Revolution

Stepper stepper;
// Setting up drivers
TMC2209Stepper driver0(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS_0);
TMC2209Stepper driver1(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS_1);
// putting them in an array for more efficiency
TMC2209Stepper *drivers[2] = {&driver0, &driver1};

// setting up FactAccel
FastAccelStepperEngine engine = FastAccelStepperEngine();
// putting them in a array for more efficiency
FastAccelStepper *steppers[2] = {nullptr, nullptr};

void Stepper::setup(int STEP0, int STEP1, int DIR0, int DIR1, int RX_PIN, int TX_PIN) {
    // Serial connection for up to 4 Drivers
    SERIAL_PORT.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
    // init FastAccelStep
    engine.init();
    int stepPins[2] = {STEP0, STEP1};
    int dirPins[2] = {DIR0, DIR1};
    // Setting up Driver
    for (int i = 0; i < 2; i++) {
        drivers[i]->begin();
        if (!drivers[i]->test_connection()) Serial.println("Driver " + String(i) + " UART error!");
        drivers[i]->toff(5); // Enables driver in software
        drivers[i]->vsense(true); // Enable UART current control
        drivers[i]->rms_current(RMS_CURRENT); // Dieser liegt bei 1,2A und Vref muss daher auch auf 1,2V
        //Microstep resolution selected by MSTEP register and NOT from the legacy pins.
        drivers[i]->mstep_reg_select(true);
        // Bei Microstepping hat man zwar eine Smoothere Drehung, aber
        // nut 70% des Drehmoments. Durch 1/3 Übersetzung haben wir dennoch 600 statt 200 Steps
        drivers[i]->microsteps(MICROSTEPS);
        drivers[i]->en_spreadCycle(true);
        drivers[i]->irun(31); // maximales Moment wenn die Motoren drehen
        // setting up Fastaccelstepper for both Steppers
        steppers[i] = engine.stepperConnectToPin(stepPins[i], 2);
        if (steppers[i]) {
            steppers[i]->setDirectionPin(dirPins[i]);
            steppers[i]->setAcceleration(5000);
        } else {
            Serial.println("Failed to initialize stepper " + String(i));
        }
    }
}


void Stepper::set(int stepperindex, int speed) {
    Serial.print("Stepper::set index: ");
    Serial.print(stepperindex);
    Serial.print(", speed: ");
    Serial.println(speed);

    if (!steppers[stepperindex]) {
        Serial.println("Stepper pointer is NULL!");
        return;
    }

    if (speed == 0) {
        steppers[stepperindex]->stopMove();
        Serial.println("Stepper stopped.");
        return;
    }

    bool direction = speed > 0;
    uint32_t mapped_speed = map(abs(speed), 0, 255, 0, REV_STEPS);

    steppers[stepperindex]->setSpeedInHz(mapped_speed);
    if (direction) {
        steppers[stepperindex]->runForward();
        Serial.println("Stepper running forward.");
    } else {
        steppers[stepperindex]->runBackward();
        Serial.println("Stepper running backward.");
    }
}
