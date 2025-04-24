#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"

#define SEALEVELPRESSURE_HPA (1013.25)

class Sensor {
    float _temp;
    float _pressure;
    float _humidity;
    float _gas;

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
