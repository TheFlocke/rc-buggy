#include <crbk_ble.h>

#include "crbk_servo.h"

const int ACT_LED = 8; // Activitiy LED for the Sensor PCB
const int SD_CSB = 9;
const int BME680_CSB = 10;
const int SPI_SDI = 11;
const int SPI_SCK = 12;
const int SPI_SDO = 13;
const int I2C_SDA = 14;
const int I2C_SCL = 15;
const int PD_INT = 16;
// GPIO Ports used to control the TMC2209
const int UART_TX = 1;
const int UART_RX = 2;
const int STEP1_DIR = 4;
const int STEP2_DIR  = 5;
const int STEP1_STEP = 6;
const int STEP2_STEP = 7;
// GPIO Ports that are not reserved by any devices
const int GPIO_17 = 17;
const int GPIO_18 = 18;
const int GPIO_21 = 21;
const int GPIO_38 = 38;
const int GPIO_47 = 47;
const int GPIO_48 = 48;

CRBKServo servo(D6); 

const int LED1=D0;
const int LED2=D9; // LOW Active

void setup() {
  pinMode(MOTOR1A, OUTPUT);
  pinMode(MOTOR1B, OUTPUT);
  pinMode(MOTOR2A, OUTPUT);
  pinMode(MOTOR2B, OUTPUT);

  pinMode(MOTOR3A, OUTPUT);
  pinMode(MOTOR3B, OUTPUT);
  pinMode(MOTOR4A, OUTPUT);
  pinMode(MOTOR4B, OUTPUT);

  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);

  digitalWrite(LED1, LOW);
  digitalWrite(LED2, HIGH);

  crbkRCCarBLE.setup("CRBK-RCCAR-6");

  servo.set(0);
}

void loop() { 
  crbkRCCarBLE.handle();
  int speed = crbkRCCarBLE.getSpeed();
  int left  = abs(speed);
  int right = abs(speed);
  int direction = crbkRCCarBLE.getDirection();
  if(direction<0) {
    // reduziere Geschwindigkeit nach links
    left -= left*(-direction/90.0);
  } else if(direction>0) {
    // reduziere Geschwindigkeit nach rechts
    right -= right*(direction/90.0);
  }
  if(speed>0) {
    // forward
    pinMode(MOTOR1B, OUTPUT);
    pinMode(MOTOR2B, OUTPUT);

    pinMode(MOTOR3B, OUTPUT);
    pinMode(MOTOR4B, OUTPUT);

    analogWrite(MOTOR1A, left);
    digitalWrite(MOTOR1B, LOW);

    analogWrite(MOTOR2A, right);
    digitalWrite(MOTOR2B, LOW);

    analogWrite(MOTOR3A, left);
    digitalWrite(MOTOR3B, LOW);

    analogWrite(MOTOR4A, right);
    digitalWrite(MOTOR4B, LOW);
  } else if(speed<0) {
    // backward
    pinMode(MOTOR1A, OUTPUT);
    pinMode(MOTOR2A, OUTPUT);

    pinMode(MOTOR3A, OUTPUT);
    pinMode(MOTOR4A, OUTPUT);

    analogWrite(MOTOR1B, left);
    digitalWrite(MOTOR1A, LOW);

    analogWrite(MOTOR2B, right);
    digitalWrite(MOTOR2A, LOW);

    analogWrite(MOTOR3B, left);
    digitalWrite(MOTOR3A, LOW);

    analogWrite(MOTOR4B, right);
    digitalWrite(MOTOR4A, LOW);
  } else {
    // stopp
    pinMode(MOTOR1A, OUTPUT);
    pinMode(MOTOR1B, OUTPUT);
    pinMode(MOTOR2A, OUTPUT);
    pinMode(MOTOR2B, OUTPUT);

    pinMode(MOTOR3A, OUTPUT);
    pinMode(MOTOR3B, OUTPUT);
    pinMode(MOTOR4A, OUTPUT);
    pinMode(MOTOR4B, OUTPUT);

    digitalWrite(MOTOR1A, LOW);
    digitalWrite(MOTOR1B, LOW);

    digitalWrite(MOTOR2A, LOW);
    digitalWrite(MOTOR2B, LOW);

    digitalWrite(MOTOR3A, LOW);
    digitalWrite(MOTOR3B, LOW);

    digitalWrite(MOTOR4A, LOW);
    digitalWrite(MOTOR4B, LOW);
  }

  digitalWrite(LED1, crbkRCCarBLE.getLED1() ? HIGH : LOW);
  digitalWrite(LED2, crbkRCCarBLE.getLED2() ? LOW : HIGH);

  servo.set(crbkRCCarBLE.getDirection());
}
