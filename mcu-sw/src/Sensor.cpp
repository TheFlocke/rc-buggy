#include "../lib/Sensor.h"

Sensor sensor;

Adafruit_BME680 bme680(&I2CBUS);

void Sensor::setup(int LED) {
    bme680.begin();
    // Set up oversampling and filter initialization
    bme680.setTemperatureOversampling(BME680_OS_8X);
    bme680.setHumidityOversampling(BME680_OS_2X);
    bme680.setPressureOversampling(BME680_OS_4X);
    bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme680.setGasHeater(320, 150); // 320*C for 150 ms
    pinMode(LED, OUTPUT);
}

String Sensor::read() {
    unsigned long endTime = bme680.beginReading();
    if (endTime == 0) {
        Serial.println("BME680 read failed - Check Wiring");
    }
    if (!bme680.endReading()) {
        Serial.println("BME680 read failed - Check Wiring");

    }
    _temp = bme680.readTemperature();
    _pressure = bme680.readPressure();
    _humidity = bme680.readHumidity();
    _gas = bme680.readGas();

    return _temp + ':' + _pressure + ':' + _humidity + ':' + _gas;
}


