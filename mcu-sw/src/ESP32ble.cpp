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

// Gas Resistance
class SensorGasResCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getSensorGasRes());
        pCharacteristic->notify();
    }
};

// Gas Index
class SensorGasIndexCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getSensorGasIndex());
        pCharacteristic->notify();
    }
};

// Gas Index
class SensorStatus : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getSensorStatus());
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
    if (_pCharCmdDriveState) {
        _pCharCmdDriveState->setValue(getDrive());
        _pCharCmdDriveState->notify();
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

    if (_pCharCmdArmState) {
        _pCharCmdArmState->setValue(getArm());
        _pCharCmdArmState->notify();
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
String ESP32ble::getSensorGasRes() const {
    return String(_gas_res);
}

void ESP32ble::setSensorGasRes(const String &value) {
    _gas_res = value;

    if (_pCharSensorGasRes) {
        _pCharSensorGasRes->setValue(getSensorGasRes());
        _pCharSensorGasRes->notify();
    }
}

// Gas Index
String ESP32ble::getSensorGasIndex() const {
    return String(_gas_index);
}

void ESP32ble::setSensorGasIndex(const String &value) {
    _gas_index = value;

    if (_pCharSensorGasIndex) {
        _pCharSensorGasIndex->setValue(getSensorGasIndex());
        _pCharSensorGasIndex->notify();
    }
}

// Gas Resistance
String ESP32ble::getSensorStatus() const {
    return String(_status);
}

void ESP32ble::setSensorStatus(const String &value) {
    _status= value;

    if (_pCharSensorStatus) {
        _pCharSensorStatus->setValue(getSensorStatus());
        _pCharSensorStatus->notify();
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
    _pCharCmdDriveState = pCmdService->createCharacteristic(
        UUID_CHAR_CMD_DRIVE_STATE,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharCmdDriveState->setCallbacks(new CmdDriveStateCallbacks());


    // Robotarm
    // Create a BLE ARM Characteristic
    // only used one time in function
    NimBLECharacteristic *pCmdArmCharacteristic = pCmdService->createCharacteristic(
        UUID_CHAR_CMD_ARM,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    pCmdArmCharacteristic->setCallbacks(new CmdArmCallbacks());

    // Create a BLE ARM_STATE Characteristic
    _pCharCmdArmState = pCmdService->createCharacteristic(
        UUID_CHAR_CMD_ARM_STATE,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharCmdArmState->setCallbacks(new CmdArmStateCallbacks());


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
    _pCharSensorGasRes = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_GAS_RES,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharSensorGasRes->setCallbacks(new SensorGasResCallbacks());

    // Create a BLE Sensor Gas Index Characteristic
    _pCharSensorGasIndex = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_GAS_INDEX,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharSensorGasIndex->setCallbacks(new SensorGasIndexCallbacks());

    // Create a BLE Sensor Status Characteristic
    _pCharSensorStatus = pSensorService->createCharacteristic(
        UUID_CHAR_SENSOR_STATUS,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCharSensorStatus->setCallbacks(new SensorGasIndexCallbacks());


    // Start the Sensor Service
    pSensorService->start();


    // Start advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName(_name.c_str());
    pAdvertising->addServiceUUID(UUID_SERVICE_CMD);
    pAdvertising->addServiceUUID(UUID_SERVICE_SENSOR);
    NimBLEDevice::startAdvertising();
}
