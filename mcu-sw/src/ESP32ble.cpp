#include "../lib/ESP32ble.h"
#include "NimBLEDevice.h"

ESP32ble esp32ble;

class CmdCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        esp32ble.setCmd(pCharacteristic->getValue());
    }
};

class StateCmdCallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        pCharacteristic->setValue(esp32ble.getCmd());
        pCharacteristic->notify();
    }
};

class ArmCallbacks : public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
        esp32ble.setArm(pCharacteristic->getValue());
    }
};

class StateArmCallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        pCharacteristic->setValue(esp32ble.getArm());
        pCharacteristic->notify();
    }
};

class SensorCallbacks : public BLECharacteristicCallbacks {
    void onRead(BLECharacteristic *pCharacteristic) {
        pCharacteristic->setValue(esp32ble.getSensor());
        pCharacteristic->notify();
    }
};

class ServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer *pServer) {
        esp32ble.onConnect();
    };

    void onDisconnect(BLEServer *pServer) {
        esp32ble.onDisconnect();
    }
};

String ESP32ble::getCmd() const {
    return "Speed_1: " + String(_speed1) + ", Speed_2: " + String(_speed2) + "Wheel_1: " + String(_wheel1) + ", Wheel_2: " + String(_wheel2);
}

void ESP32ble::setCmd(String value) {
    String cmd = value;

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
        if (_wheel1 > 180) _wheel1 = 180;
        if (_wheel1 < 0) _wheel1 = 0;
        if (_wheel2 > 180) _wheel2 = 180;
        if (_wheel2 < 0) _wheel2 = 0;
    } else {
        _speed1 = 0;
        _speed2 = 0;
        // Wheel wird einfach auf der letzten Position gelassen
        }


    if (_pCmdStateCharacteristic) {
        _pCmdStateCharacteristic->setValue(getCmd());
        _pCmdStateCharacteristic->notify();
    }
}

String ESP32ble::getArm() const {
    return "Servo_1: " + String(_arm1) + ", Servo_2: " + String(_arm2) + ", Servo_3: " + String(_arm3) + ", Gripper: " + String(_arm4);
}

void ESP32ble::setArm(String value) {
    String cmd = value;

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
}

String ESP32ble::getSensor() const {
    return "Temp: " + String(_temp) + ", Humidity: " + String(_humidity) + ", Pressure: " + String(_pressure) + ", Gas: " + String(_gas);
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

    // Creates BLE Descriptor 0x2904: Client Characteristic Configuration Descriptor (CCCD) (needed for notify)
    pCmdCharacteristic->addDescriptor(new BLE2904());
    pCmdCharacteristic->setCallbacks(new CmdCallbacks()); {
        // Adds also the Characteristic Type Description - 0x2904 descriptor
        BLE2904 *descriptor_2904 = new BLE2904();
        descriptor_2904->setFormat(BLE2904::FORMAT_UTF8);
        pCmdCharacteristic->addDescriptor(descriptor_2904);
    }

    // Create a BLE STATE_CMD Characteristic
    _pCmdStateCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_STATE_CMD,
        NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pCmdStateCharacteristic->setCallbacks(new StateCmdCallbacks()); {
        // Creates BLE Descriptor 0x2902: Client Characteristic Configuration Descriptor (CCCD) (needed for notify)
        _pCmdStateCharacteristic->addDescriptor(new BLE2904());
        // Adds also the Characteristic Type Description - 0x2904 descriptor
        BLE2904 *descriptor_2904 = new BLE2904();
        descriptor_2904->setFormat(BLE2904::FORMAT_UTF8);
        _pCmdStateCharacteristic->addDescriptor(descriptor_2904);
    }

    // Create a BLE SENSOR Characteristic
    _pSensorCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_SENSOR,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pSensorCharacteristic->setCallbacks(new SensorCallbacks()); {
        // Creates BLE Descriptor 0x2902: Client Characteristic Configuration Descriptor (CCCD) (needed for notify)
        _pSensorCharacteristic->addDescriptor(new BLE2904());
        // Adds also the Characteristic Type Description - 0x2904 descriptor
        BLE2904 *descriptor_2904 = new BLE2904();
        descriptor_2904->setFormat(BLE2904::FORMAT_UTF8);
        _pSensorCharacteristic->addDescriptor(descriptor_2904);
    }

    // Create a BLE ARM Characteristic
    _pArmCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_ARM,
        NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pArmCharacteristic->setCallbacks(new ArmCallbacks()); {
        // Creates BLE Descriptor 0x2902: Client Characteristic Configuration Descriptor (CCCD) (needed for notify)
        _pArmCharacteristic->addDescriptor(new BLE2904());
        // Adds also the Characteristic Type Description - 0x2904 descriptor
        BLE2904 *descriptor_2904 = new BLE2904();
        descriptor_2904->setFormat(BLE2904::FORMAT_UTF8);
        _pArmCharacteristic->addDescriptor(descriptor_2904);
    }

    // Create a BLE ARM_STATE Characteristic
    _pStateArmCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_STATE_ARM,
        NIMBLE_PROPERTY::NOTIFY | NIMBLE_PROPERTY::INDICATE
    );
    _pStateArmCharacteristic->setCallbacks(new StateArmCallbacks()); {
        // Creates BLE Descriptor 0x2902: Client Characteristic Configuration Descriptor (CCCD) (needed for notify)
        _pStateArmCharacteristic->addDescriptor(new BLE2904());
        // Adds also the Characteristic Type Description - 0x2904 descriptor
        BLE2904 *descriptor_2904 = new BLE2904();
        descriptor_2904->setFormat(BLE2904::FORMAT_UTF8);
        _pStateArmCharacteristic->addDescriptor(descriptor_2904);
    }

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    BLEDevice::startAdvertising();
}

void ESP32ble::handle() {
    // disconnecting
    if (!_connected && _lastConnectionState) {
        delay(500); // give the bluetooth stack the chance to get things ready
        BLEDevice::startAdvertising(); // restart advertising
        _lastConnectionState = _connected;
    }
    // connecting
    if (_connected && !_lastConnectionState) {
        _lastConnectionState = _connected;
    }
}
