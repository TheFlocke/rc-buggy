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
    int _led = -1;
    unsigned long _endTime = 0;
    bool _reading_started = false;


public:
    // Add this new method to check reading status

    // Keep existing methods the same
    void setup(int LED);
    bool beginRead(); // New method to start reading
    bool endRead();   // New method to finish reading
    void read();      // Keep for compatibility

    static String float2string(float value);

    String getTemp() const { return _temp; }
    String getHumidity() const { return _humidity; }
    String getPressure() const { return _pressure; }
    String getGas() const { return _gas; }
    bool isReadingStarted() { return _reading_started; }
};

extern Sensor sensor;
#endif //SENSOR_H
