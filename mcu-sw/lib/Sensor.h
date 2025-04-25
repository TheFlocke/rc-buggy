#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "../lib/i2c_bus.h"


class Sensor {
    float _temp = -1;
    float _pressure = -1;
    float _humidity = -1;
    float _gas = -1;

    public:

    void setup();

    void read();

    float getTemp() const { return _temp; }
    float getPressure() const { return _pressure; }
    float getHumidity() const { return _humidity; }
    float getGas() const { return _gas; }
};

extern Sensor sensor;
#endif //SENSOR_H
