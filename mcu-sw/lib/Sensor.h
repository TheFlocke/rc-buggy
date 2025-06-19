#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <bme68xLibrary.h>
#include "../lib/i2c_bus.h"


class Sensor {
    String _temp{"N/A"};
    String _pressure{"N/A"};
    String _humidity{"N/A"};
    String _gas_res{"N/A"};
    String _gas_index{"N/A"};
    String _status{"N/A"};
    int _led = -1;
    unsigned long _endTime = 0;
    bool _reading_started = false;

public:
    void setup(int LED);

    bool read();

    static String float2string(float value);

    String getTemp() const { return _temp; }
    String getHumidity() const { return _humidity; }
    String getPressure() const { return _pressure; }
    String getGasRes() const { return _gas_res; }
    String getGasIndex() const { return _gas_index; }
    String getStatus() const { return _status; }
};

extern Sensor sensor;
#endif //SENSOR_H
