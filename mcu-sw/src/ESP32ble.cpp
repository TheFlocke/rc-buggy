#include "../lib/ESP32ble.h"
#include "NimBLEDevice.h"

ESP32ble esp32ble;
// vars and array init
int speed0 = -1;
int speed1 = -1;
int wheel0 = -1;
int wheel1 = -1;
int arm0 = -1;
int arm1 = -1;
int arm2 = -1;
int arm3 = -1;


int *vars[8] = {&speed0, &speed1, &wheel0, &wheel1, &arm0, &arm1, &arm2, &arm3};


// CMD Callbacks
class CmdCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        esp32ble.setCMD(pCharacteristic->getValue());
        pCharacteristic->notify();
    }
};

class CmdStateCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getCMD());
        pCharacteristic->notify();
    }
};


// Sensor Callbacks
// Temperature
class SensorTempCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getSensorTemp());
        pCharacteristic->notify();
    }
};

// Humidity
class SensorHumidityCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getSensorHumidity());
        pCharacteristic->notify();
    }
};

// Pressure
class SensorPressureCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getSensorPressure());
        pCharacteristic->notify();
    }
};

// IAQ
class SensorIAQCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getSensorIAQ());
        pCharacteristic->notify();
    }
};

// CO2
class SensorCO2Callbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getSensorCO2());
        pCharacteristic->notify();
    }
};

// VOC
class SensorVOCCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getSensorVOC());
        pCharacteristic->notify();
    }
};


// Server Callbacks
class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override {
        esp32ble.onConnect();
    };

    void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override {
        esp32ble.onDisconnect();
    }
};

void ESP32ble::onConnect() {
    _connected = true;
}

void ESP32ble::onDisconnect() {
    _connected = false;
}

void ESP32ble::handle() {
    if (!_connected && _lastConnectionState && !_waitingToAdvertise) {
        _disconnectTime = millis();
        _waitingToAdvertise = true;
    }

    if (_waitingToAdvertise && millis() - _disconnectTime >= 500) {
        NimBLEDevice::startAdvertising();
        _lastConnectionState = _connected;
        _waitingToAdvertise = false;
    }

    if (_connected && !_lastConnectionState) {
        _lastConnectionState = _connected;
    }
}


String ESP32ble::getCMD() const {
    return String(_latestID) + ":" + String(*vars[_latestID]);
}

void ESP32ble::setCMD(const String &cmd) {
    int sepIdx = cmd.indexOf(':');
    if (sepIdx == -1) return; // Invalid format

    // get id and value out of String
    int id = cmd.substring(0, sepIdx).toInt();
    int value = cmd.substring(sepIdx + 1).toInt();

    // array: Each ID corresponding to its usage
    int idxs[10] = {0, 1, 0, 1, 2, 3, 4, 5, 6, 7};

    // Safety check
    if (id < 0 || id >= 8) return;

    // only update value if has changed
    if (*vars[id] != value) {
        *vars[id] = value;

        // connect Value to corresponding ID
        if (id <= 1) {
            stepper.set(idxs[id], *vars[id]);
        } else if (id <= 7) {
            Servo::set(idxs[id], *vars[id]);
            if (idxs[id] == 1 || idxs[id] == 3) {
                Servo::set(idxs[id + 1], 180 - *vars[id]);
            }
        }
        // update latest value that was updated so
        _latestID = id;
        if (_pCharCmdState) {
            _pCharCmdState->setValue(getCMD());
            _pCharCmdState->notify();
        }
    }
}


// Sensor
// Temperature
String ESP32ble::getSensorTemp() const {
    return String(_temp);
}

void ESP32ble::setSensorTemp(const String &value) {
    _temp = value;

    if (_pCharSensorTemp) {
        _pCharSensorTemp->setValue(getSensorTemp());
        _pCharSensorTemp->notify();
    }
}

// Humidity
String ESP32ble::getSensorHumidity() const {
    return String(_humidity);
}

void ESP32ble::setSensorHumidity(const String &value) {
    _humidity = value;

    if (_pCharSensorHumidity) {
        _pCharSensorHumidity->setValue(getSensorHumidity());
        _pCharSensorHumidity->notify();
    }
}

// Pressure
String ESP32ble::getSensorPressure() const {
    return String(_pressure);
}

void ESP32ble::setSensorPressure(const String &value) {
    _pressure = value;

    if (_pCharSensorPressure) {
        _pCharSensorPressure->setValue(getSensorPressure());
        _pCharSensorPressure->notify();
    }
}

// Gas Resistance
String ESP32ble::getSensorIAQ() const {
    return String(_iaq);
}

void ESP32ble::setSensorIAQ(const String &value) {
    _iaq = value;

    if (_pCharSensorIAQ) {
        _pCharSensorIAQ->setValue(getSensorIAQ());
        _pCharSensorIAQ->notify();
    }
}

// Gas Index
String ESP32ble::getSensorCO2() const {
    return String(_co2);
}

void ESP32ble::setSensorCO2(const String &value) {
    _co2 = value;

    if (_pCharSensorCO2) {
        _pCharSensorCO2->setValue(getSensorCO2());
        _pCharSensorCO2->notify();
    }
}

// Gas Resistance
String ESP32ble::getSensorVOC() const {
    return String(_voc);
}

void ESP32ble::setSensorVOC(const String &value) {
    _voc = value;

    if (_pCharSensorVOC) {
        _pCharSensorVOC->setValue(getSensorVOC());
        _pCharSensorVOC->notify();
    }
}


void ESP32ble::setup(const String &name) {
    // Create the BLE Device
    this->_name = name;
    NimBLEDevice::init(name.c_str());

    // Create the BLE Server
    _pServer = NimBLEDevice::createServer();
    _pServer->setCallbacks(new ServerCallbacks());

    // Create the BLE CMDService
    NimBLEService *pCmdService = _pServer->createService(UUID_SERVICE_CMD);

    // Create the BLE SensorService
    NimBLEService *pSensorService = _pServer->createService(UUID_SERVICE_SENSOR);


    // Wheels/Drive
    // Create a BLE CMD Characteristic
    // only used one time in function
    NimBLECharacteristic *pCmdCharacteristic = pCmdService->createCharacteristic(
        UUID_CHAR_CMD,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    pCmdCharacteristic->setCallbacks(new CmdCallbacks());


    // Create a BLE CmdDriveState Characteristic
    _pCharCmdState = pCmdService->createCharacteristic(
        UUID_CHAR_CMD_STATE,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharCmdState->setCallbacks(new CmdStateCallbacks());


    // Start the CMD Service
    pCmdService->start();


    // Sensor
    // Create a BLE Sensor Temp Characteristic
    _pCharSensorTemp = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_TEMP,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharSensorTemp->setCallbacks(new SensorTempCallbacks());

    // Create a BLE Sensor Humidity Characteristic
    _pCharSensorHumidity = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_HUMIDITY,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharSensorHumidity->setCallbacks(new SensorHumidityCallbacks());

    // Create a BLE Sensor Pressure Characteristic
    _pCharSensorPressure = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_PRESSURE,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharSensorPressure->setCallbacks(new SensorPressureCallbacks());

    // Create a BLE Sensor Gas Resistance Characteristic
    _pCharSensorIAQ = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_IAQ,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharSensorIAQ->setCallbacks(new SensorIAQCallbacks());

    // Create a BLE Sensor Gas Index Characteristic
    _pCharSensorCO2 = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_CO2,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharSensorCO2->setCallbacks(new SensorCO2Callbacks());

    // Create a BLE Sensor Status Characteristic
    _pCharSensorVOC = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_VOC,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharSensorVOC->setCallbacks(new SensorVOCCallbacks());


    // Start the Sensor Service
    pSensorService->start();


    // Start advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName(_name.c_str());
    pAdvertising->addServiceUUID(UUID_SERVICE_CMD);
    pAdvertising->addServiceUUID(UUID_SERVICE_SENSOR);
    NimBLEDevice::startAdvertising();
}
