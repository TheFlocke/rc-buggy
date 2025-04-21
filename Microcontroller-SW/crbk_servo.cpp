#include "Arduino.h"
#include "crbk_servo.h"

#define RESOLUTION 8 // in bit 
#define FREQUENCY 50  // in Hz

// SG90: 0.10s/60°
// kurze Pulse: <1ms -90°, Mittelstellung 1,5ms, 90° >2ms
// Pulweite 20ms=255 -> 1ms=12,75, 19,125; 25.5
// PWM-Pulsweite 500-2400 µs -> 6,38; 18,5; 30,6 bei 8 Bit(max 255)
// PWM-Pulsweite 500-2400 µs -> 25; 74; 122 bei 10 Bit(max 1024)

CRBKServo::CRBKServo(int pin) : _pin(pin) {
  pinMode(_pin, OUTPUT);
   analogWriteResolution(_pin, RESOLUTION);
   analogWriteFrequency(_pin, FREQUENCY);
  // ledcAttach(_pin, LEDC_FREQUENCY, LEDC_RESOLUTION);
    // _duty =  (pow(2, LEDC_RESOLUTION) - 1) * 0.075; //  {0.075,0.087,0.075,0.0625};
    //5.1 is the ratio of pulse width (in decimilliseconds)to duty cycle at a fequency of 200Hz (the minimum for ledc on esp32c3)
}

void CRBKServo::set(int angle){
  // int duty=(map(angle,0,180,5,25))*5.1;
  // int duty=(map(angle,-90,90,27,127));
  int duty=(map(angle,-90,90,27,127));
  duty=map(angle,-10,10,0,255);

  // digitalWrite(D6, HIGH);
  // delay(angle);
  // digitalWrite(D6, LOW);
  // delay(20-angle);


  // ledcWrite(_pin,duty);//write to servo
  analogWrite(_pin, duty);
}

void CRBKServo::up(){
    // ledcWrite(0,77); //90degress
    // ledcWrite(0,128);//180 degrees
    // delay(300);
    // ledcWrite(0,77);//90 degrees
}

void CRBKServo::down(){
    // ledcWrite(0,77); //90degress
    // ledcWrite(0,26);//0 degrees
    // delay(300);
    // ledcWrite(0,77);//90 degrees
}