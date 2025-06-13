#include "../lib/Sensor.h"
#include "../lib/main.h"

Sensor sensor;

Bme68x bme680;

#define NEW_GAS_MEAS (BME68X_GASM_VALID_MSK | BME68X_HEAT_STAB_MSK | BME68X_NEW_DATA_MSK)

void Sensor::setup(int LED) {
    // Sensor itself
    bme680.begin(0x77, I2CBUS);
    if (bme680.checkStatus()) {
        if (bme680.checkStatus() == BME68X_ERROR) {
            Serial.println("Sensor error:" + bme680.statusString());
            return;
        }
        if (bme680.checkStatus() == BME68X_WARNING) {
            Serial.println("Sensor Warning:" + bme680.statusString());
        }
    }
    // All default values provided by Bosch Sensortec Library
    /* Set the default configuration for temperature, pressure and humidity */
    bme680.setTPH();
    /* Heater temperature in degree Celsius */
    uint16_t tempProf[10] = {
        320, 100, 100, 100, 200, 200, 200, 320, 320,
        320
    };
    /* Multiplier to the shared heater duration */
    uint16_t mulProf[10] = {5, 2, 10, 30, 5, 5, 5, 5, 5, 5};
    /* Shared heating duration in milliseconds */
    uint16_t sharedHeaterDur = MEAS_DUR - (bme680.getMeasDur(BME68X_PARALLEL_MODE) / 1000);

    bme680.setHeaterProf(tempProf, mulProf, sharedHeaterDur, 10);
    // All measurements are taken in Parallel instead of sequentially
    bme680.setOpMode(BME68X_PARALLEL_MODE);

    // LED on Sensor board
    _led = LED;
    pinMode(LED, OUTPUT);
}

bool Sensor::read() {
    bme68xData bme680data;
    uint8_t nFieldsLeft = 0;


    if (bme680.fetchData()) {
        do {
            nFieldsLeft = bme680.getData(bme680data);
            if (bme680data.status == NEW_GAS_MEAS) {
                _temp = bme680data.temperature;
                _pressure = bme680data.pressure;
                _humidity = bme680data.humidity;
                _gas_res = bme680data.gas_resistance;
                _status = bme680data.status;
                _gas_index = bme680data.gas_index;
            }
        } while (nFieldsLeft);
        return true;
    }
    return false;
}


String Sensor::float2string(float value) {
    // 15 Characters -1 for null and -1 for -x
    char buffer[16];
    // use buffer as safe space
    // 3 ==> overall minimum 3 digits including decimal point
    // 2 ==> after decimal point 2 digits
    // f ==> convert float to String
    snprintf(buffer, sizeof(buffer), "%4.4f", value);
    return {buffer};
}
