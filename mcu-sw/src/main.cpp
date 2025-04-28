#include <Arduino.h>
#include "../lib/ESP32ble.h"
#include "../lib/i2c_bus.h"
#include "../lib/Servo.h"
#include "../lib/Sensor.h"
#include "../lib/Stepper.h"

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
constexpr int UART_TX = 1;
constexpr int UART_RX = 2;
constexpr int STEP1_DIR = 4;
constexpr int STEP2_DIR  = 5;
constexpr int STEP1_STEP = 6;
constexpr int STEP2_STEP = 7;
// GPIO Ports that are not reserved by any devices
[[maybe_unused]] constexpr int GPIO_17 = 17;
[[maybe_unused]] constexpr int GPIO_18 = 18;
[[maybe_unused]] constexpr int GPIO_21 = 21;
[[maybe_unused]] constexpr int GPIO_38 = 38;
[[maybe_unused]] constexpr int GPIO_47 = 47;
[[maybe_unused]] constexpr int GPIO_48 = 48;

// Using I2C 0 bus of 2 on the esp32
TwoWire I2CBUS = TwoWire(0);

void setup() {
  // Loading and setting Serial for communiction for Motordriver up
  stepper.setup();
  // for debbuging
  // Serial.begin(9600);
  // Giving ESP32 a BLE name
  esp32ble.setup("rc-rover");
  // Initalazing I2C with predefined Ports
  I2CBUS.begin(I2C_SDA, I2C_SCL, 100000);
  // Loading Servo Setup and executing it ==> to see more go to ../src/servo.cpp
  servo.setup();
  // Loading and setting Sensor up with LED set to ACT_LED
  sensor.setup(ACT_LED);
}

void loop() {
  esp32ble.handle();

  // Pull all the Int for later use from the Webinterface
  // Left Wheel
  int speed1 = esp32ble.getSpeed1();
  int wheel1 = esp32ble.getWheel1();
  // Right Wheel
  int speed2 = esp32ble.getSpeed2();
  int wheel2 = esp32ble.getWheel2();
/*
  servo.set(5, wheel1);
  servo.set(6, wheel2);
  */
  stepper.stepper_0(speed1);
 // stepper.stepper_1(speed2);
  Serial.print(speed1);

  // Arm
  int arm1 = esp32ble.getArm1();
  int arm2 = esp32ble.getArm2();
  int arm3 = esp32ble.getArm3();
  int grabber = esp32ble.getArm4();
/*
  servo.set(0, arm1);
  servo.set(1, arm2);
  servo.set(2, arm3);
  servo.set(3, grabber);
*/
  // Put here the Control Mechanism for the Speed

  float temp = sensor.getTemp();
  float pressure = sensor.getPressure();
  float humidity = sensor.getHumidity();
  float gas = sensor.getGas();

  /*
  sensor.read();
  Serial.print("temp: ");
  Serial.println(temp);
  Serial.print("pressure: ");
  Serial.println(pressure);
  Serial.print("humidity: ");
  Serial.println(humidity);
  Serial.print("gas: ");
  Serial.println(gas);
  delay(500);
  */
}