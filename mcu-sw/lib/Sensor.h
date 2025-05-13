#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include "Adafruit_BME680.h"
#include "../lib/i2c_bus.h"


class Sensor {
    String _temp{"-1"};
    String _pressure{"-1"};
    String _humidity{"-1"};
    String _gas{"-1"};

    public:

    static void setup(int LED);

    bool read();
    static String float2string(float value);

    String getTemp() const { return _temp; }
    String getHumidity() const { return _humidity; }
    String getPressure() const { return _pressure; }
    String getGas() const { return _gas; }
};

extern Sensor sensor;
#endif //SENSOR_H
