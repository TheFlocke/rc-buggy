#include <Arduino.h>
#include "../lib/Stepper.h"


// Your Driver settings
#define SERIAL_PORT Serial1 // TMC2208/TMC2224 HardwareSerial port
#define DRIVER_ADDRESS_0 0b00 // TMC2209 Driver address according to MS1 and MS2
#define DRIVER_ADDRESS_1 0b01 // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.17f // for 1.2A see online on how to calc https://learn.watterott.com/silentstepstick/faq/#calculator-tmc21xx-and-tmc2209

// Your motor parts:
#define FULL_STEPS 200.0   // 1.8 degrees per whole step
#define MICROSTEPS 8     // Usually 16.0 but at 1 ==> Fullstep
#define GEAR_REDUCTION 3.0       // Gearbox translation
#define RMS_CURRENT 1200  // in mA max 2.1A

// Don't edit these:
#define REV_STEPS            (FULL_STEPS * MICROSTEPS * GEAR_REDUCTION) // How many Steps are needed for one Revolution

Stepper stepper;

TMC2209Stepper driver0(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS_0);
TMC2209Stepper driver1(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS_1);
FastAccelStepperEngine engine = FastAccelStepperEngine();

void Stepper::setup(int STEP0, int STEP1, int DIR0, int DIR1, int RX_PIN, int TX_PIN) {
    // getting Data from the local Variables
    _PSTEP0_STEP = STEP0;
    _PSTEP1_STEP = STEP1;
    _PSTEP0_Dir = DIR0;
    _PSTEP1_Dir = DIR1;
    _RX_PIN = RX_PIN;
    _TX_PIN = TX_PIN;

    // Serial connection for up to 4 Drivers
    Serial2.begin(115200, SERIAL_8N1, _RX_PIN, _TX_PIN);
    // Setting up Driver
    engine.init();
    // UART: Init SW UART (if selected) with default 115200 baudrate
    driver0.toff(5); // Enables driver in software
    driver1.toff(5);
    driver0.vsense(true); // Enable UART current control
    driver1.vsense(true);
    driver0.rms_current(RMS_CURRENT); // Dieser liegt bei 1,2A und Vref muss daher auch auf 1,2V
    driver1.rms_current(RMS_CURRENT);
    //Microstep resolution selected by MSTEP register and NOT from the legacy pins.
    driver0.mstep_reg_select(true);
    driver1.mstep_reg_select(true);
    // Bei Microstepping hat man zwar eine Smoothere Drehung, aber
    // nut 70% des Drehmoments. Durch 1/3 Übersetzung haben wir dennoch 600 statt 200 Steps
    driver0.microsteps(MICROSTEPS);
    driver1.microsteps(MICROSTEPS);
    driver0.en_spreadCycle(true);
    driver1.en_spreadCycle(true);
    // Setting up Library that helps to control the Speed and Acceleration
    stepper0 = engine.stepperConnectToPin(_PSTEP0_STEP,2); // First is the Pin and second is the driver Type: 0 -> MCPWM, 1 -> RMT, 2 -> both ; creates a Stepper
    if (stepper0) {
        stepper0->setDirectionPin(_PSTEP0_Dir);
        stepper0->setAcceleration(5000); // steps/s^2
    } else {
        Serial.println("Failed to initialize stepper!");
    }
    // Setting up Library that helps to control the Speed and Acceleration
    stepper1 = engine.stepperConnectToPin(_PSTEP1_STEP,2); // First is the Pin and second is the driver Type: 0 -> MCPWM, 1 -> RMT, 2 -> both ; creates a Stepper
    if (stepper1) {
        stepper1->setDirectionPin(_PSTEP1_Dir);
        stepper1->setAcceleration(5000); // steps/s^2
    } else {
        Serial.println("Failed to initialize stepper!");
    }
}

void Stepper::stepper_0(int speed) {
    if (speed == 0) {
        stepper0->forceStop();
    } else {
        if (speed > 0) {
            stepper0->setSpeedInHz(map(speed, 0, 255, 0, REV_STEPS));  // the last two speeds set the maximum speed
            stepper0->runBackward();
        }
        if (speed < 0) {
            stepper0->setSpeedInHz(map(speed, 0, -255, 0, REV_STEPS));  // the last two speeds set the maximum speed
            stepper0->runForward();
        }
    }
}

void Stepper::stepper_1(int speed) {
    if (speed == 0) {
        stepper1->forceStop();
    } else {
        if (speed > 0) {
            stepper1->setSpeedInHz(map(speed, 0, 255, 0, REV_STEPS));  // the last two speeds set the maximum speed
            stepper1->runForward();
        }
        if (speed < 0) {
            stepper1->setSpeedInHz(map(speed, 0, -255, 0, REV_STEPS));  // the last two speeds set the maximum speed
            stepper1->runBackward();
        }
    }
}
