#include "NimBLEDevice.h"

// See the following for generating UUIDs: https://www.uuidgenerator.net/
#define SERVICE_UUID                "5eaf1079-e806-47a9-a1ec-d815bea94805"
#define CHARACTERISTIC_CMD          "7cb6bbe0-f35e-4a34-a8e2-6731102e12e3"
#define CHARACTERISTIC_STATE_CMD    "bd6fbfde-385d-480f-b5eb-64d60cc7be9a"
#define CHARACTERISTIC_SENSOR       "4a95c0ef-4ee8-420d-8c35-c643678f7b77"
#define CHARACTERISTIC_ARM          "99d69805-8efb-450e-ae78-c4ddba09f7f6"
#define CHARACTERISTIC_STATE_ARM    "f8765d0c-81b5-4780-85a4-44f0999f5474"


class ESP32ble {
    String _name;
    bool _connected = false;
    bool _lastConnectionState = false;
    int _speed = 0;
    int _direction = 0;

    BLEServer *_pServer = nullptr;
    BLECharacteristic *_pCmdStateCharacteristic = nullptr;
    BLECharacteristic *_pSensorCharacteristic = nullptr;
    BLECharacteristic *_pArmCharacteristic = nullptr;
    BLECharacteristic *_pStateArmCharacteristic = nullptr;

public:
    void setup(String name);

    void handle();

    void onConnect();

    void onDisconnect();

    void setCmd(String value);

    String getCmd() const;

    //-90° ... 90°
    int getDirection() const { return _direction; }
    //-255 ... 255
    int getSpeed() const { return _speed; }


    int getDriveMode() const;

    enum {
        STOPPED,
        FORWARD,
        LEFTFORWARD,
        RIGHTFORWARD,
        BACKWARD,
        LEFTBACKWARD,
        RIGHTBACKWARD
    };
};

extern ESP32ble esp32ble;
