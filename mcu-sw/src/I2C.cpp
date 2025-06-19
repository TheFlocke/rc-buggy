//
// Created by Philipp Winterberg on 19.06.25.
//

#include "../lib/I2C.h"

I2C i2c;

// init i2c mutex
// Semaphore for I2C so tasks (who dont know about each other) wont talk parallel
SemaphoreHandle_t i2cMutex = nullptr;

// Using I2C 0 bus of 2 on the esp32
TwoWire I2CBUS = TwoWire(0);

void I2C::setup(int SDA, int SCL) {
    // Create Semaphore Mutex for I2C
    i2cMutex = xSemaphoreCreateMutex();
    // Initializing I2C with predefined Ports
    I2CBUS.begin(SDA, SCL, 100000);
}
