#include "../lib/ESP32ble.h"
#include "NimBLEDevice.h"

ESP32ble esp32ble;

class CmdCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        esp32ble.setCmd(pCharacteristic->getValue());
    }
};

class StateCmdCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getCmd());
    }
};

class ArmCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        esp32ble.setArm(pCharacteristic->getValue());
    }
};

class StateArmCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getArm());
    }
};

class SensorCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getSensor());
    }
};

class ServerCallbacks : public NimBLEServerCallbacks {
    void onConnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo) override {
        esp32ble.onConnect();
    };

    void onDisconnect(NimBLEServer *pServer, NimBLEConnInfo &connInfo, int reason) override {
        esp32ble.onDisconnect();
    }
};

String ESP32ble::getCmd() const {
    return String(_speed1) + ":" + String(_speed2) + ":" + String(_wheel1) + ":" + String(_wheel2);
}

void ESP32ble::setCmd(String value) {
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
        _speed1 = 0;
        _speed2 = 0;
        // Wheel wird einfach auf der letzten Position gelassen
    }


    if (_pStateCmdCharacteristic) {
        _pStateCmdCharacteristic->setValue(getCmd());
    }
}

String ESP32ble::getArm() const {
    return  String(_arm1) + ":" + String(_arm2) + ":" + String(_arm3) + ":" + String(_arm4);
}

void ESP32ble::setArm(String value) {
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

    // sonst soll er die Values so lassen ==> daher nichts hingeschrieben
    if (_pStateArmCharacteristic) {
        _pStateArmCharacteristic->setValue(getArm());
    }
}

String ESP32ble::getSensor() const {
    return String(_temp) + ":" + String(_humidity) + ":" + String(_pressure) + ":" + String(_gas);
}

void ESP32ble::onConnect() {
    _connected = true;
}

void ESP32ble::onDisconnect() {
    _connected = false;
}

void ESP32ble::setup(String name) {
    // Create the BLE Device
    this->_name = name;
    NimBLEDevice::init(name.c_str());

    // Create the BLE Server
    _pServer = NimBLEDevice::createServer();
    _pServer->setCallbacks(new ServerCallbacks());

    // Create the BLE Service
    NimBLEService *pService = _pServer->createService(SERVICE_UUID);

    // Create a BLE CMD Characteristic
    NimBLECharacteristic *pCmdCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_CMD,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE
    );
    pCmdCharacteristic->setCallbacks(new CmdCallbacks()); {
        // Adds also the Characteristic Type Description - 0x2904 descriptor
        NimBLE2904 *descriptor_2904 = pCmdCharacteristic->create2904();
        descriptor_2904->setFormat(NimBLE2904::FORMAT_UTF8);
        pCmdCharacteristic->addDescriptor(descriptor_2904);
    }

    // Create a BLE STATE_CMD Characteristic
    _pStateCmdCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_STATE_CMD,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE
    );
    _pStateCmdCharacteristic->setCallbacks(new StateCmdCallbacks()); {
        // Adds also the Characteristic Type Description - 0x2904 descriptor
        NimBLE2904 *descriptor_2904 = pCmdCharacteristic->create2904();
        descriptor_2904->setFormat(NimBLE2904::FORMAT_UTF8);
        _pStateCmdCharacteristic->addDescriptor(descriptor_2904);
    }

    // Create a BLE SENSOR Characteristic
    _pSensorCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_SENSOR,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE
    );
    _pSensorCharacteristic->setCallbacks(new SensorCallbacks()); {
        // Adds also the Characteristic Type Description - 0x2904 descriptor
        NimBLE2904 *descriptor_2904 = pCmdCharacteristic->create2904();
        descriptor_2904->setFormat(NimBLE2904::FORMAT_UTF8);
        _pSensorCharacteristic->addDescriptor(descriptor_2904);
    }

    // Create a BLE ARM Characteristic
    _pArmCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_ARM,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::INDICATE
    );
    _pArmCharacteristic->setCallbacks(new ArmCallbacks()); {
        // Adds also the Characteristic Type Description - 0x2904 descriptor
        NimBLE2904 *descriptor_2904 = pCmdCharacteristic->create2904();
        descriptor_2904->setFormat(NimBLE2904::FORMAT_UTF8);
        _pArmCharacteristic->addDescriptor(descriptor_2904);
    }

    // Create a BLE ARM_STATE Characteristic
    _pStateArmCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_STATE_ARM,
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::INDICATE
    );
    _pStateArmCharacteristic->setCallbacks(new StateArmCallbacks()); {
        // Adds also the Characteristic Type Description - 0x2904 descriptor
        NimBLE2904 *descriptor_2904 = pCmdCharacteristic->create2904();
        descriptor_2904->setFormat(NimBLE2904::FORMAT_UTF8);
        _pStateArmCharacteristic->addDescriptor(descriptor_2904);
    }

    // Start the service
    pService->start();

    // Start advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->setName(_name.c_str());
    pAdvertising->addServiceUUID(SERVICE_UUID);
    NimBLEDevice::startAdvertising();
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
