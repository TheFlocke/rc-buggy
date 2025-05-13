#include <Arduino.h>
#include "../lib/ESP32ble.h"
#include "../lib/i2c_bus.h"
#include "../lib/Servo.h"
#include "../lib/Sensor.h"
#include "../lib/Stepper.h"
/*
constexpr int ACT_LED = 8; // Activitiy LED for the Sensor PCB
constexpr int SD_CSB = 9;
constexpr int BME680_CSB = 10;
constexpr int SPI_SDI = 11;
constexpr int SPI_SCK = 12;
constexpr int SPI_SDO = 13;
constexpr int I2C_SDA = 14;
constexpr int I2C_SCL = 15;
constexpr int PD_INT = 16;
// GPIO Ports used to control the TMC2209
constexpr int UART_TX = 2;
constexpr int UART_RX = 1;
constexpr int STEP0_DIR = 4;
constexpr int STEP1_DIR  = 5;
constexpr int STEP0_STEP = 6;
constexpr int STEP1_STEP = 7;
// GPIO Ports that are not reserved by any devices
[[maybe_unused]] constexpr int GPIO_17 = 17;
[[maybe_unused]] constexpr int GPIO_18 = 18;
[[maybe_unused]] constexpr int GPIO_21 = 21;
[[maybe_unused]] constexpr int GPIO_38 = 38;
[[maybe_unused]] constexpr int GPIO_47 = 47;
[[maybe_unused]] constexpr int GPIO_48 = 48;
*/

constexpr int ACT_LED = 20; // Activitiy LED for the Sensor PCB
constexpr int I2C_SDA = 1;
constexpr int I2C_SCL = 2;
// GPIO Ports used to control the TMC2209
constexpr int UART_TX = 16;
constexpr int UART_RX = 15;
constexpr int STEP0_DIR = 4;
constexpr int STEP1_DIR  = 6;
constexpr int STEP0_STEP = 5;
constexpr int STEP1_STEP = 7;




// Using I2C 0 bus of 2 on the esp32
TwoWire I2CBUS = TwoWire(0);

void setup() {
  // for debugging
  // Serial.begin(9600);
  // Giving ESP32 a BLE name
  esp32ble.setup("rc-rover");
  // Initializing I2C with predefined Ports
  I2CBUS.begin(I2C_SDA, I2C_SCL, 100000);
  // Loading Servo Setup and executing it ==> to see more go to ../src/servo.cpp
  Servo::setup();
  // Loading and setting Serial for communication for Motordriver up. Also setting Pins for STEP and DIR
  stepper.setup(STEP0_STEP, STEP1_STEP, STEP0_DIR, STEP1_DIR, UART_RX, UART_TX);
  // Loading and setting Sensor up with LED set to ACT_LED
  Sensor::setup(ACT_LED);
}

void loop() {
  esp32ble.handle();

  // Wheels
  Servo::set(6, esp32ble.getWheel1());
  Servo::set(7, esp32ble.getWheel2());
  stepper.stepper_0(esp32ble.getSpeed1());
  stepper.stepper_1(esp32ble.getSpeed2());

  // Arm
  int arm1 = esp32ble.getArm1();
  int arm2 = esp32ble.getArm2();
  int arm3 = esp32ble.getArm3();
  int grabber = esp32ble.getArm4();

  Servo::set(0, arm1);
  Servo::set(1, map(arm1, 0, 180, 180, 0));
  Servo::set(2, arm2);
  Servo::set(3, map(arm2, 0, 180, 180, 0));
  Servo::set(4, arm3);
  Servo::set(5, grabber);

  // Sensor
  // Sense all data and convert it to Strings
  sensor.read();
  // Send all the converted Data to Client
  esp32ble.setSensorTemp(sensor.getTemp());
  esp32ble.setSensorHumidity(sensor.getHumidity());
  esp32ble.setSensorPressure(sensor.getPressure());
  esp32ble.setSensorGas(sensor.getGas());
}
