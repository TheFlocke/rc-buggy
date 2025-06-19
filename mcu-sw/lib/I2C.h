//
// Created by Philipp Winterberg on 24.04.25.
//

#ifndef I2C_BUS_H
#define I2C_BUS_H

#include <Wire.h>

class I2C {

public:
    void setup(int SDA, int SCL);
};

extern I2C i2c;

extern SemaphoreHandle_t i2cMutex;

extern TwoWire I2CBUS;


#endif //I2C_BUS_H
