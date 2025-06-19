//
// Created by Philipp Winterberg on 13.06.25.
//

#ifndef MAIN_H
#define MAIN_H

#include <Arduino.h>
#include "../lib/ESP32ble.h"
#include "../lib/i2c_bus.h"
#include "../lib/Servo.h"
#include "../lib/Sensor.h"
#include "../lib/Stepper.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

class main {
    // Measure duration in ms



public:
    void sensorTask(void *xTaskParameters);
    void bleTask(void *xTaskParameters);

};

extern SemaphoreHandle_t i2cMutex;
constexpr int MEAS_DUR = 140;

#endif //MAIN_H