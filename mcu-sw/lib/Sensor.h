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

    static void setup(int LED);

    String read();
};

extern Sensor sensor;
#endif //SENSOR_H
