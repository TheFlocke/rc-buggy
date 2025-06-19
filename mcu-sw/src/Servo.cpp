#include "Arduino.h"
#include "../lib/Servo.h"
#include "../lib/I2C.h"

Servo servo;

Adafruit_PWMServoDriver pwm_servo(0x40, I2CBUS);

constexpr int servo_min = 150;
constexpr int servo_max = 650;
constexpr int servo_freq = 50;


void Servo::set(int pin, int angle) {
    int duty = map(angle, 0, 180, servo_min, servo_max);
    if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
        pwm_servo.setPWM(pin, 0, duty);
        xSemaphoreGive(i2cMutex);
    }
}

void Servo::setup() {
    pwm_servo.begin();
    pwm_servo.setPWMFreq(servo_freq);
}
