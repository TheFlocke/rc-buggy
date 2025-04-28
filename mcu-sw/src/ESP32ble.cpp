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
        pCharacteristic->notify();
    }
};

class ArmCallbacks : public NimBLECharacteristicCallbacks {
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        esp32ble.setArm(pCharacteristic->getValue());
    }
};

class StateArmCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) {
        pCharacteristic->setValue(esp32ble.getArm());
        pCharacteristic->notify();
    }
};

class SensorCallbacks : public NimBLECharacteristicCallbacks {
    void onRead(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo &connInfo) override {
        pCharacteristic->setValue(esp32ble.getSensor());
        pCharacteristic->notify();
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
    return "Speed_1: " + String(_speed1) + ", Speed_2: " + String(_speed2) + ",Wheel_1: " + String(_wheel1) +
           ", Wheel_2: " + String(_wheel2);
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

        // Frontend schickt zu große Werte ==> werden von dem Backend korrigiert
        if (_speed1 > 255) _speed1 = 255;
        if (_speed1 < -255) _speed1 = -255;
        if (_speed2 > 255) _speed2 = 255;
        if (_speed2 < -255) _speed2 = -255;
        if (_wheel1 > 130) _wheel1 = 130;
        if (_wheel1 < 55) _wheel1 = 55;
        if (_wheel2 > 120) _wheel2 = 120;
        if (_wheel2 < 60) _wheel2 = 60;
    } else {
        _speed1 = 0;
        _speed2 = 0;
        // Wheel wird einfach auf der letzten Position gelassen
    }


    if (_pStateCmdCharacteristic) {
        _pStateCmdCharacteristic->setValue(getCmd());
        _pStateCmdCharacteristic->notify();
    }
}

String ESP32ble::getArm() const {
    return "Servo_1: " + String(_arm1) + ", Servo_2: " + String(_arm2) + ", Servo_3: " + String(_arm3) + ", Gripper: " +
           String(_arm4);
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

        // Frontend schickt zu große Werte ==> werden von dem Backend korrigiert
        if (_arm1 > 180) _arm1 = 180;
        if (_arm1 < 12) _arm1 = 12;
        if (_arm2 > 180) _arm2 = 180;
        if (_arm2 < 12) _arm2 = 12;
        if (_arm3 > 180) _arm3 = 180;
        if (_arm3 < 0) _arm3 = 0;
        if (_arm4 > 180) _arm4 = 180;
        if (_arm4 < 90) _arm4 = 90;
    }

    // sonst soll er die Values so lassen ==> daher nichts hingeschrieben
    if (_pStateArmCharacteristic) {
        _pStateArmCharacteristic->setValue(getArm());
        _pStateArmCharacteristic->notify();
    }
}

String ESP32ble::getSensor() const {
    return "Temp: " + String(_temp) + ", Humidity: " + String(_humidity) + ", Pressure: " + String(_pressure) +
           ", Gas: " + String(_gas);
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
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
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
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
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
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
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
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
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
        NIMBLE_PROPERTY::READ | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
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
    // disconnecting
    if (!_connected && _lastConnectionState) {
        delay(500); // give the bluetooth stack the chance to get things ready
        NimBLEDevice::startAdvertising(); // restart advertising
        _lastConnectionState = _connected;
    }
    // connecting
    if (_connected && !_lastConnectionState) {
        _lastConnectionState = _connected;
    }
}
