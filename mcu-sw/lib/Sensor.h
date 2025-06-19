#ifndef SENSOR_H
#define SENSOR_H

#include <Arduino.h>
#include <bsec2.h>
#include "../lib/I2C.h"


class Sensor {
    int _led = -1;

public:
    void setup(int LED);

    static void read();

};

extern Sensor sensor;
#endif //SENSOR_H
