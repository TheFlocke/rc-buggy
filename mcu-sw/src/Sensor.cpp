#include "../lib/Sensor.h"
#include "../lib/I2C.h"
#include "../lib/ESP32ble.h"

#define SAMPLE_RATE		BSEC_SAMPLE_RATE_LP

Sensor sensor;

void checkIaqSensorStatus();

void checkBsecStatus(Bsec2 bsec);

// Check if Sensor algorithm is initialised and if not say so to ble client
String checkInit(int id, String data);

bool isInit[3] = {false, false, false};

void newDataCallback(bme68xData data, bsecOutputs outputs, Bsec2 bsec);


Bsec2 bme680;


void Sensor::setup(int LED) {
    if (!bme680.begin(0x77, I2CBUS)) {
        checkBsecStatus(bme680);
    }

    if (SAMPLE_RATE == BSEC_SAMPLE_RATE_ULP) {
        bme680.setTemperatureOffset(TEMP_OFFSET_ULP);
    } else if (SAMPLE_RATE == BSEC_SAMPLE_RATE_LP) {
        bme680.setTemperatureOffset(TEMP_OFFSET_LP);
    }

    // List of data needed to be processed from the Sensor
    bsec_virtual_sensor_t sensorList[6] =
    {
        BSEC_OUTPUT_RAW_PRESSURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE,
        BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY,
        BSEC_OUTPUT_IAQ,
        BSEC_OUTPUT_CO2_EQUIVALENT,
        BSEC_OUTPUT_BREATH_VOC_EQUIVALENT,
    };

    /* Subsribe to the desired BSEC2 outputs */
    if (!bme680.updateSubscription(sensorList, ARRAY_LEN(sensorList), SAMPLE_RATE)) {
        checkBsecStatus(bme680);
    }

    /* Whenever new data is available call the newDataCallback function */
    bme680.attachCallback(newDataCallback);


    // LED on Sensor board
    _led = LED;
    pinMode(LED, OUTPUT);
}

void Sensor::read() {
    if (!bme680.run()) {
        checkBsecStatus(bme680);
    }
}

void newDataCallback(const bme68xData data, const bsecOutputs outputs, Bsec2 bsec) {
    if (xSemaphoreTake(i2cMutex, portMAX_DELAY) == pdTRUE) {
        if (!outputs.nOutputs) {
            return;
        }

        for (uint8_t i = 0; i < outputs.nOutputs; i++) {
            const bsecData output = outputs.output[i];
            switch (output.sensor_id) {
                case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_TEMPERATURE:
                    esp32ble.setSensor(0, String(output.signal));
                    break;
                case BSEC_OUTPUT_SENSOR_HEAT_COMPENSATED_HUMIDITY:
                    esp32ble.setSensor(1, String(output.signal));
                    break;
                case BSEC_OUTPUT_RAW_PRESSURE:
                    esp32ble.setSensor(2, String(output.signal));
                    break;
                case BSEC_OUTPUT_IAQ:
                    esp32ble.setSensor(3, checkInit(0, String(output.signal)));
                    break;
                case BSEC_OUTPUT_CO2_EQUIVALENT:
                    esp32ble.setSensor(4, checkInit(1, String(output.signal)));
                    break;
                case BSEC_OUTPUT_BREATH_VOC_EQUIVALENT:
                    esp32ble.setSensor(5, checkInit(2, String(output.signal)));
                    break;
                default:
                    break;
            }
        }
        xSemaphoreGive(i2cMutex);
    }
}


void checkBsecStatus(Bsec2 bsec) {
    if (bsec.status < BSEC_OK) {
        Serial.println("BSEC error code : " + String(bsec.status));
    } else if (bsec.status > BSEC_OK) {
        Serial.println("BSEC warning code : " + String(bsec.status));
    }
    if (bsec.sensor.status < BME68X_OK) {
        Serial.println("BME68X error code : " + String(bsec.sensor.status));
    } else if (bsec.sensor.status > BME68X_OK) {
        Serial.println("BME68X warning code : " + String(bsec.sensor.status));
    }
}

String checkInit(int id, String data) {
    // IAQ, CO2, VOC
    if (isInit[id]) {
        return data;
    }
    const String checkData[3] = {"50.00", "500.00", "0.50"};
    if (checkData[id] != String(data.c_str())) {
        isInit[id] = true;
        return data;
    }
    return {"Initializing..."};
}
