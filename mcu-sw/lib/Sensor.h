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

    int getTemp() const { return _temp; }
    int getHumidity() const { return _humidity; }
    int getPressure() const { return _pressure; }
    int getGas() const { return _gas; }
};

extern Sensor sensor;
#endif //SENSOR_H
