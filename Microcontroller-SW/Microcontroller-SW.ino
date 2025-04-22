#include <crbk_ble.h>
#include "TMC2209.h"
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

// initializing motorrdriver(s)
// Important: For NEMA 17 motors, the current is in general in the range of 0.5A to 0.8A RMS, which is a reference voltage (Vref) of 0.7V to 1.1V.
TMC2209 stepper_driver_left;
const TMC2209::SerialAddress SERIAL_ADDRESS_left = TMC2209::SERIAL_ADDRESS_left; // defining Serial address assigned by the library as a constant
TMC2209 stepper_driver_right;
const TMC2209::SerialAddress SERIAL_ADDRESS_right = TMC2209::SERIAL_ADDRESS_right; // defining Serial address assigned by the library as a constant
const uint8_t REPLY_DELAY = 4;
const long SERIAL_BAUD_RATE = 115200;


void setup() {
// setting up serial connection for communication with motordriver(s)
  stepper_driver_left.setup(Serial1,SERIAL_BAUD_RATE,SERIAL_ADDRESS_left,UART_RX,UART_TX);
  stepper_driver_left.setReplyDelay(REPLY_DELAY);
  stepper_driver_right.setup(Serial1,SERIAL_BAUD_RATE,SERIAL_ADDRESS_right,UART_RX,UART_TX);
  stepper_driver_right.setReplyDelay(REPLY_DELAY);
// Setting pinMode for for motor-control
  pinMode(STEP1_DIR, OUTPUT);
  pinMode(STEP2_DIR, OUTPUT);
  pinMode(STEP1_STEP, OUTPUT);
  pinMode(STEP2_STEP, OUTPUT);
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
}
