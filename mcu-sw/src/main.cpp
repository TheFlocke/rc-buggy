#include "../lib/main.h"


/*
constexpr int ACT_LED = 8; // Activity LED for the Sensor PCB
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

constexpr int ACT_LED = 20; // Activity LED for the Sensor PCB
constexpr int I2C_SDA = 16;
constexpr int I2C_SCL = 15;
// GPIO Ports used to control the TMC2209
constexpr int UART_TX = 20;
constexpr int UART_RX = 21;
constexpr int STEP0_DIR = 4;
constexpr int STEP1_DIR = 6;
constexpr int STEP0_STEP = 5;
constexpr int STEP1_STEP = 7;

// for servos
int arm0;
int arm1;
int arm2;
int arm3;
int wheel0;
int wheel1;

// for steppers
int speed0;
int speed1;


// Using I2C 0 bus of 2 on the esp32
TwoWire I2CBUS = TwoWire(0);

// Create a Task extra for the Sensor so that it won't block the loop and cause latency issues
TaskHandle_t sensorTaskHandle = nullptr;
TaskHandle_t BLETaskHandle = nullptr;
TaskHandle_t stepperTaskHandle = nullptr;

// Semaphore for I2C so tasks (who dont know about each other) wont talk parallel
SemaphoreHandle_t i2cMutex = nullptr;

void sensorTask(void *xTaskParameters) {
    for (;;) {
        /* data being fetched for every 140ms */
        vTaskDelay(MEAS_DUR / portTICK_RATE_MS);
        do {
            esp32ble.setSensorTemp(sensor.getTemp());
            esp32ble.setSensorHumidity(sensor.getHumidity());
            esp32ble.setSensorPressure(sensor.getPressure());
            esp32ble.setSensorGasRes(sensor.getGasRes());
            esp32ble.setSensorGasIndex(sensor.getGasIndex());
            esp32ble.setSensorStatus(sensor.getStatus());
        } while (sensor.read());
    };
};

void bleTask(void *xTaskParameters) {
    for (;;) {
        esp32ble.handle();
        vTaskDelay(10);
    }
}

void stepperTask(void *xTaskParameters) {
    for (;;) {
        if (speed0 != esp32ble.getSpeed0()) {
            speed0 = esp32ble.getSpeed0();
            stepper.stepper_0(-1 * speed0); // links
        }
        if (speed1 != esp32ble.getSpeed1()) {
            speed1 = esp32ble.getSpeed1();
            stepper.stepper_1(-1 * speed1); // Rechts
        }
        vTaskDelay(5);
    }
}


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
    sensor.setup(ACT_LED);
    // Create sensor task pinned to Core 0
    xTaskCreate(bleTask, "BLETask", 4096, nullptr, 1, &BLETaskHandle);
    xTaskCreate(servoTask, "ServoTask", 8192, nullptr, 2, &servoTaskHandle);
    xTaskCreate(stepperTask, "StepperTask", 4096, nullptr, 3, &stepperTaskHandle);
    xTaskCreate(sensorTask, "SensorTask", 4096, nullptr, 3, &sensorTaskHandle);
    // Create Semaphore Mutex for I2C
    i2cMutex = xSemaphoreCreateMutex();
}


void loop() {
    delay(10);
}
