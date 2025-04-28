#include <Arduino.h>
#include "../lib/Stepper.h"


// Your Driver settings
constexpr int RX_PIN = 1;
constexpr int TX_PIN = 2;
#define SERIAL_PORT Serial1 // TMC2208/TMC2224 HardwareSerial port
#define DRIVER_ADDRESS_0 0b00 // TMC2209 Driver address according to MS1 and MS2
#define DRIVER_ADDRESS_1 0b01 // TMC2209 Driver address according to MS1 and MS2
#define R_SENSE 0.12f // for 1.2A see online on how to calc https://all3dp.com/2/vref-calculator-tmc2209-tmc2208-a4988/

// Your motor parts:
#define FULL_STEPS 200.0   // 1.8 degrees per whole step
#define MICROSTEPS 0     // Usually 16.0
#define GEAR_REDUCTION 3.0       // Gearbox translation
#define RMS_CURRENT 1200  // in mA max 2.1A

// Don't edit these:
#define REV_STEPS            (FULL_STEPS /* * MICROSTEPS */ * GEAR_REDUCTION) // How many Steps are needed for one Revolution

Stepper stepper;

TMC2209Stepper driver0(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS_0);
TMC2209Stepper driver1(&SERIAL_PORT, R_SENSE, DRIVER_ADDRESS_1);
AccelStepper stepper0(AccelStepper::DRIVER, 6, 4);
AccelStepper stepper1(AccelStepper::DRIVER, 7, 5);


void Stepper::setup() {
    // Serial connection for up to 4 Drivers
    Serial1.begin(115200, SERIAL_8N1, RX_PIN, TX_PIN);
    // Setting up Driver
    driver0.begin(); //  SPI: Init CS pins and possible SW SPI pins
    driver1.begin();
    // UART: Init SW UART (if selected) with default 115200 baudrate
    driver0.toff(5); // Enables driver in software
    driver1.toff(5);
    driver0.vsense(true); // Enable UART current control
    driver1.vsense(true);
    driver0.rms_current(RMS_CURRENT); // Dieser liegt bei 1,2A und Vref muss daher auch auf 1,2V
    driver1.rms_current(RMS_CURRENT);
    driver0.microsteps(MICROSTEPS); // Bei Microstepping hat man zwar eine Smoothere Drehung, aber
    driver1.microsteps(MICROSTEPS);
    // nut 70% des Drehmoments. Durch 1/3 Übersetzung haben wir dennoch 600 statt 200 Steps
    driver0.en_spreadCycle(true);
    driver1.en_spreadCycle(true);
    // Setting up Libary that helps to control the Speed and Acceleration
    stepper0.setMaxSpeed(5000); // max Steps where to Driver and motor are working (tested) because of the libary used
    stepper1.setMaxSpeed(5000); // max Steps where to Driver and motor are working (tested)
    stepper0.setAcceleration(2000); // 2000mm/s^2
    stepper1.setAcceleration(2000); // 2000mm/s^2
    stepper0.setPinsInverted(false, false, true);
    stepper1.setPinsInverted(false, false, true);
    stepper0.enableOutputs();
    stepper1.enableOutputs();
}

void Stepper::stepper_0(int speed) {
    stepper0.setSpeed(map(speed, -255, 255, -1 * REV_STEPS * 5, REV_STEPS * 5));
    stepper0.run();
}

void Stepper::stepper_1(int speed) {
    stepper1.setSpeed(map(speed, -255, 255, -1 * REV_STEPS , REV_STEPS));
    stepper1.run();
}
