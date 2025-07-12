#include "../lib/ESP32ble.h"
#include "NimBLEDevice.h"

ESP32ble esp32ble;
// vars and array init
// CMD
int speed0 = -1;
int speed1 = -1;
int wheel0 = -1;
int wheel1 = -1;
int arm0 = -1;
int arm1 = -1;
int arm2 = -1;
int arm3 = -1;
// Sensor
String temp{"N/A"};
String pressure{"N/A"};
String humidity{"N/A"};
String iaq{"N/A"};
String co2{"N/A"};
String voc{"N/A"};


int *cmdValues[8] = {&speed0, &speed1, &wheel0, &wheel1, &arm0, &arm1, &arm2, &arm3};
String *sensorValues[6] = {&temp, &pressure, &humidity, &iaq, &co2, &voc};


// CMD Callbacks
class CmdCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        esp32ble.setCMD(pCharacteristic->getValue());
        pCharacteristic->notify();
    }
};

// Only used for Debugging
// class CmdStateCallbacks : public NimBLECharacteristicCallbacks {
//     void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
//         pCharacteristic->setValue(esp32ble.getCMD());
//         pCharacteristic->notify();
//     }
// };


// Sensor Callbacks
class SensorValueCallback : public NimBLECharacteristicCallbacks {
public:
    using GetterFunc = std::function<String(void)>;

    SensorValueCallback(GetterFunc getter) : _getter(getter) {
    }

    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(_getter());
        pCharacteristic->notify();
    }

private:
    GetterFunc _getter;
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
    return String(_latestID) + ":" + String(*cmdValues[_latestID]);
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
    if (*cmdValues[id] != value) {
        *cmdValues[id] = value;

        // connect Value to corresponding ID
        if (id <= 1) {
            stepper.set(idxs[id], *cmdValues[id]);
        }
        if (id >= 2) {
            Servo::set(idxs[id], *cmdValues[id]);
            if (idxs[id] == 1 || idxs[id] == 3) {
                Servo::set(idxs[id + 1], 180 - *cmdValues[id]);
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

void ESP32ble::setSensor(int id, const String &value) {
    if (id < 0 || id >= 6) return; // Safety check

    if (*sensorValues[id] != value) {
        *sensorValues[id] = value;
        if (_sensorCharacteristics[id]) {
            _sensorCharacteristics[id]->setValue(getSensor(id));
            _sensorCharacteristics[id]->notify();
        }
    }
}


// Sensor
String ESP32ble::getSensor(const int id) {
    if (id < 0 || id >= 6) return "Error: Data is uut of scope!"; // Safety check

    return String(*sensorValues[id]);
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

    // only used for debugging
    // Create a BLE CmdState Characteristic
    // _pCharCmdState = pCmdService->createCharacteristic(
    //     UUID_CHAR_CMD_STATE,
    //     NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    // );
    // _pCharCmdState->setCallbacks(new CmdStateCallbacks());


    // Sensor
    // Create a BLE Sensor Temp Characteristic
    const char *sensorCharUUID[6] = {
        UUID_CHAR_SENSOR_TEMP,
        UUID_CHAR_SENSOR_PRESSURE,
        UUID_CHAR_SENSOR_HUMIDITY,
        UUID_CHAR_SENSOR_IAQ,
        UUID_CHAR_SENSOR_CO2,
        UUID_CHAR_SENSOR_VOC
    };
    for (int id = 0; id < 6; id++) {
        _sensorCharacteristics[id] = pSensorService->createCharacteristic(
            sensorCharUUID[id],
            NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
        );
        _sensorCharacteristics[id]->setCallbacks(new SensorValueCallback([id]() {
            return esp32ble.getSensor(id);
        }));
    }

    pCmdService->start();
    // Start the Sensor Service
    pSensorService->start();

    // Start advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName(_name.c_str());
    pAdvertising->addServiceUUID(UUID_SERVICE_CMD);
    pAdvertising->addServiceUUID(UUID_SERVICE_SENSOR);
    NimBLEDevice::startAdvertising();
}
