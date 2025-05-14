#include "../lib/Sensor.h"

Sensor sensor;

Adafruit_BME680 bme680(&I2CBUS);

void Sensor::setup(int LED) {
    _led = LED;
    bme680.begin();
    // Set up oversampling and filter initialization
    bme680.setTemperatureOversampling(BME680_OS_8X);
    bme680.setHumidityOversampling(BME680_OS_2X);
    bme680.setPressureOversampling(BME680_OS_4X);
    bme680.setIIRFilterSize(BME680_FILTER_SIZE_3);
    bme680.setGasHeater(320, 150); // 320*C for 150 ms
    pinMode(LED, OUTPUT);
}

// Start a non-blocking read operation
bool Sensor::beginRead() {
    if (!_reading_started) {
        digitalWrite(_led, HIGH);  // Turn on activity LED
        _endTime = bme680.beginReading();
        if (_endTime == 0) {
            Serial.println("BME680 read failed - Check Wiring");
            digitalWrite(_led, LOW);
            return false;
        }
        _reading_started = true;
        return true;
    }
    return false;
}

// Check if reading is complete and process data if it is
bool Sensor::endRead() {
    if (_reading_started) {
        if (bme680.endReading()) {
            // Process data
            _temp = float2string(bme680.temperature);
            _pressure = float2string(bme680.pressure / 100.0); // Convert to hPa
            _humidity = float2string(bme680.humidity);
            _gas = float2string(bme680.gas_resistance);
            _reading_started = false;
            digitalWrite(_led, LOW);  // Turn off activity LED
            return true;
        }
    }
    return false;
}

// Keep the original read method for compatibility, but implement it as non-blocking
void Sensor::read() {
    // For compatibility with existing code, but now uses non-blocking approach
    if (!_reading_started) {
        beginRead();
    } else {
        endRead();
    }
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
