#include "NimBLEDevice.h"

// See the following for generating UUIDs: https://www.uuidgenerator.net/
#define SERVICE_UUID                "5eaf1079-e806-47a9-a1ec-d815bea94805"
#define CHARACTERISTIC_CMD          "7cb6bbe0-f35e-4a34-a8e2-6731102e12e3"
#define CHARACTERISTIC_STATE_CMD    "bd6fbfde-385d-480f-b5eb-64d60cc7be9a"
#define CHARACTERISTIC_SENSOR       "4a95c0ef-4ee8-420d-8c35-c643678f7b77"
#define CHARACTERISTIC_ARM          "99d69805-8efb-450e-ae78-c4ddba09f7f6"
#define CHARACTERISTIC_STATE_ARM    "f8765d0c-81b5-4780-85a4-44f0999f5474"


class ESP32ble {
    // everything for BLE connection
    String _name;
    bool _connected = false;
    bool _lastConnectionState = false;
    // everything for the movement of the rover
    int _speed1 = 0;
    int _speed2 = 0;
    int _wheel1 = 45;
    int _wheel2 = 45;
    // everything for the movement of the arm (default set to home)
    int _arm1 = 45;
    int _arm2 = 45;
    int _arm3 = 45;
    int _arm4 = 45;
    // everything from the sensor
    int _temp = -1;
    int _humidity = -1;
    int _pressure = -1;
    int _gas = -1;
    // ESP32 handle
    unsigned long _disconnectTime = 0;
    bool _waitingToAdvertise = false;

    NimBLEServer *_pServer = nullptr;
    NimBLECharacteristic *_pStateCmdCharacteristic = nullptr;
    NimBLECharacteristic *_pSensorCharacteristic = nullptr;
    NimBLECharacteristic *_pArmCharacteristic = nullptr;
    NimBLECharacteristic *_pStateArmCharacteristic = nullptr;

public:
    void setup(const String &name);

    void handle();

    void onConnect();

    void onDisconnect();

    void setCmd(const String &value);

    String getCmd() const;

    String getArm() const;

    void setArm(const String &value);

    String getSensor() const;

    //-255 ... 255
    int getSpeed1() const { return _speed1; }
    int getSpeed2() const { return _speed2; }
    // Einstellungswert der Stollen
    int getWheel1() const { return _wheel1; }
    int getWheel2() const { return _wheel2; }
    // Einstellungswert der einzelnen Armelemente zwischen 0 bis 180
    // Grenzwerte sind zur Sicherheit festgelegt, da sonst die Motoren durch Dauerlast durchbrennen oder der Arm kaput geht
    int getArm1() const { return _arm1; }
    int getArm2() const { return _arm2; }
    int getArm3() const { return _arm3; }
    int getArm4() const { return _arm4; }


};

extern ESP32ble esp32ble;
