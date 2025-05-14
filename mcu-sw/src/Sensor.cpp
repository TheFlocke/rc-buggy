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

bool Sensor::beginReading() {
    if (_reading_started) return false;

    _endTime = bme680.beginReading();
    if (_endTime == 0) {
        Serial.println("BME680 read failed - Check Wiring");
        return false;
    }
    _reading_started = true;
    return true;
}

bool Sensor::checkComplete() {
    if (!_reading_started) return false;

    if (millis() < _endTime) return false;

    if (!bme680.endReading()) {
        Serial.println("BME680 read failed - Check Wiring");
        _reading_started = false;
        return false;
    }

    // Process data
    _temp = float2string(bme680.readTemperature());
    _pressure = float2string(bme680.readPressure());
    _humidity = float2string(bme680.readHumidity());
    _gas = float2string(bme680.readGas());

    _reading_started = false;
    return true;
}


String Sensor::float2string(float value) {
    // 15 Characters -1 for null and -1 for -x
    char buffer[16];
    // use buffer as save space
    // 3 ==> overall minimum 3 digits including decimal point
    // 2 ==> after decimal point 2 digits
    // f ==> convert float to String
    snprintf(buffer, sizeof(buffer), "%4.4f", value);
    return {buffer};
}
