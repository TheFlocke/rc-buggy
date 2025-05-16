#include "../lib/ESP32ble.h"
#include "NimBLEDevice.h"

ESP32ble esp32ble;


// CMD Callbacks
// Drive
class CmdDriveCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        esp32ble.setDrive(pCharacteristic->getValue());
        pCharacteristic->notify();
    }
};

class CmdDriveStateCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getDrive());
        pCharacteristic->notify();
    }
};

// Arm
class CmdArmCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        esp32ble.setArm(pCharacteristic->getValue());
        pCharacteristic->notify();
    }
};

class CmdArmStateCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getArm());
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

// Gas
class SensorGasCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getSensorGas());
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


String ESP32ble::getDrive() const {
    return String(_speed1) + ":" + String(_speed2) + ":" + String(_wheel1) + ":" + String(_wheel2);
}

void ESP32ble::setDrive(const String &value) {
    // prüfen auf kombinierte anweisung: // `${speed1}:${speed2}:${wheel1}:${wheel2}`
    int t1 = value.indexOf(":");
    int t2 = value.indexOf(":", t1 + 1);
    int t3 = value.indexOf(":", t2 + 1);
    if (t1 > 0 && t3 < value.length()) {
        _speed1 = value.substring(0, t1).toInt();
        _speed2 = value.substring(t1 + 1, t2).toInt();
        _wheel1 = value.substring(t2 + 1, t3).toInt();
        _wheel2 = value.substring(t3 + 1).toInt();
    } else {
        // Wheel wird einfach auf der letzten Position gelassen
        _speed1 = 0;
        _speed2 = 0;
    }
    if (_pCharacteristicCmdDriveState) {
        _pCharacteristicCmdDriveState->setValue(getDrive());
        _pCharacteristicCmdDriveState->notify();
    }
}

String ESP32ble::getArm() const {
    return String(_arm1) + ":" + String(_arm2) + ":" + String(_arm3) + ":" + String(_arm4);
}

void ESP32ble::setArm(const String &value) {
    // prüfen auf kombinierte anweisung: // `${arm1}:${arm2}:${arm3}:${arm4}`
    int t1 = value.indexOf(":");
    int t2 = value.indexOf(":", t1 + 1);
    int t3 = value.indexOf(":", t2 + 1);
    if (t1 > 0 && t3 < value.length()) {
        _arm1 = value.substring(0, t1).toInt();
        _arm2 = value.substring(t1 + 1, t2).toInt();
        _arm3 = value.substring(t2 + 1, t3).toInt();
        _arm4 = value.substring(t3 + 1).toInt();
    }

    if (_pCharacteristicCmdArmState) {
        _pCharacteristicCmdArmState->setValue(getArm());
        _pCharacteristicCmdArmState->notify();
    }
}

// Sensor
// Temperature
String ESP32ble::getSensorTemp() const {
    return String(_temp);
}

void ESP32ble::setSensorTemp(const String &value) {
    _temp = value;

    if (_pCharacteristicSensorTemp) {
        _pCharacteristicSensorTemp->setValue(getSensorTemp());
        _pCharacteristicSensorTemp->notify();
    }
}

// Humidity
String ESP32ble::getSensorHumidity() const {
    return String(_humidity);
}

void ESP32ble::setSensorHumidity(const String &value) {
    _humidity = value;

    if (_pCharacteristicSensorHumidity) {
        _pCharacteristicSensorHumidity->setValue(getSensorHumidity());
        _pCharacteristicSensorHumidity->notify();
    }
}

// Pressure
String ESP32ble::getSensorPressure() const {
    return String(_pressure);
}

void ESP32ble::setSensorPressure(const String &value) {
    _pressure = value;

    if (_pCharacteristicSensorPressure) {
        _pCharacteristicSensorPressure->setValue(getSensorPressure());
        _pCharacteristicSensorPressure->notify();
    }
}

// Gas
String ESP32ble::getSensorGas() const {
    return String(_gas);
}

void ESP32ble::setSensorGas(const String &value) {
    _gas = value;

    if (_pCharacteristicSensorGas) {
        _pCharacteristicSensorGas->setValue(getSensorGas());
        _pCharacteristicSensorGas->notify();
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
    NimBLECharacteristic *pCmdDriveCharacteristic = pCmdService->createCharacteristic(
        UUID_CHAR_CMD_DRIVE,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    pCmdDriveCharacteristic->setCallbacks(new CmdDriveCallbacks());


    // Create a BLE CmdDriveState Characteristic
    _pCharacteristicCmdDriveState = pCmdService->createCharacteristic(
        UUID_CHAR_CMD_DRIVE_STATE,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharacteristicCmdDriveState->setCallbacks(new CmdDriveStateCallbacks());


    // Robotarm
    // Create a BLE ARM Characteristic
    // only used one time in function
    NimBLECharacteristic *pCmdArmCharacteristic = pCmdService->createCharacteristic(
        UUID_CHAR_CMD_ARM,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    pCmdArmCharacteristic->setCallbacks(new CmdArmCallbacks());

    // Create a BLE ARM_STATE Characteristic
    _pCharacteristicCmdArmState = pCmdService->createCharacteristic(
        UUID_CHAR_CMD_ARM_STATE,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharacteristicCmdArmState->setCallbacks(new CmdArmStateCallbacks());


    // Start the CMD Service
    pCmdService->start();


    // Sensor
    // Create a BLE Sensor Temp Characteristic
    _pCharacteristicSensorTemp = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_TEMP,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharacteristicSensorTemp->setCallbacks(new SensorTempCallbacks());

    // Create a BLE Sensor Humidity Characteristic
    _pCharacteristicSensorHumidity = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_HUMIDITY,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharacteristicSensorHumidity->setCallbacks(new SensorHumidityCallbacks());

    // Create a BLE Sensor Pressure Characteristic
    _pCharacteristicSensorPressure = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_PRESSURE,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharacteristicSensorPressure->setCallbacks(new SensorPressureCallbacks());

    // Create a BLE Sensor Gas Characteristic
    _pCharacteristicSensorGas = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_GAS,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharacteristicSensorGas->setCallbacks(new SensorGasCallbacks());


    // Start the Sensor Service
    pSensorService->start();


    // Start advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName(_name.c_str());
    pAdvertising->addServiceUUID(UUID_SERVICE_CMD);
    pAdvertising->addServiceUUID(UUID_SERVICE_SENSOR);
    NimBLEDevice::startAdvertising();
}
