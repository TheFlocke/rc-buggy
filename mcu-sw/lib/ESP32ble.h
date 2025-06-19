#include "NimBLEDevice.h"
#include "../lib/Servo.h"
#include "../lib/Stepper.h"

// See the following for generating UUIDs: https://www.uuidgenerator.net/
// CMD Service
#define UUID_SERVICE_CMD                        "5eaf1079-e806-47a9-a1ec-d815bea94805"
#define UUID_CHAR_CMD_DRIVE                     "7cb6bbe0-f35e-4a34-a8e2-6731102e12e3"
#define UUID_CHAR_CMD_DRIVE_STATE               "bd6fbfde-385d-480f-b5eb-64d60cc7be9a"
#define UUID_CHAR_CMD_ARM                       "99d69805-8efb-450e-ae78-c4ddba09f7f6"
#define UUID_CHAR_CMD_ARM_STATE                 "f8765d0c-81b5-4780-85a4-44f0999f5474"
// Sensor Service
#define UUID_SERVICE_SENSOR                     "8da7a992-e263-4b78-abf2-bdb94808895c"
#define UUID_CHAR_SENSOR_TEMP                   "24c53354-de00-42ac-926c-31f805e5d2f5"
#define UUID_CHAR_SENSOR_PRESSURE               "545343fb-93a0-4415-9e2b-6a4c8e2835c4"
#define UUID_CHAR_SENSOR_HUMIDITY               "618c8e95-e436-4a86-9d7d-17c3db9992d0"
#define UUID_CHAR_SENSOR_IAQ                    "aa4ce7cf-fff0-4d54-b4ec-fb24920b35c1"
#define UUID_CHAR_SENSOR_CO2                    "3b9e4e45-42f2-4892-8d63-35f4a4bc8093"
#define UUID_CHAR_SENSOR_VOC                    "988b016d-91a0-4830-ae55-650ef2bb9c8d"


class ESP32ble {
    // everything for BLE connection
    String _name;
    bool _connected = false;
    bool _lastConnectionState = false;
    // everything for the movement of the rover
    int _speed0 = -1;
    int _speed1 = -1;
    int _wheel0 = -1;
    int _wheel1 = -1;
    // everything for the movement of the arm (default set to home)
    int _arm0 = -1;
    int _arm1 = -1;
    int _arm2 = -1;
    int _arm3 = -1;
    // everything from the sensor
    String _temp{"N/A"};
    String _pressure{"N/A"};
    String _humidity{"N/A"};
    String _iaq{"N/A"};
    String _co2{"N/A"};
    String _voc{"N/A"};
    // ESP32 handle
    unsigned long _disconnectTime = 0;
    bool _waitingToAdvertise = false;

    NimBLEServer *_pServer = nullptr;

    // CMD Service
    NimBLECharacteristic *_pCharCmdDriveState = nullptr;
    NimBLECharacteristic *_pCharCmdArmState = nullptr;

    // Sensor Service
    NimBLECharacteristic *_pCharSensorTemp = nullptr;
    NimBLECharacteristic *_pCharSensorHumidity = nullptr;
    NimBLECharacteristic *_pCharSensorPressure = nullptr;
    NimBLECharacteristic *_pCharSensorIAQ = nullptr;
    NimBLECharacteristic *_pCharSensorCO2 = nullptr;
    NimBLECharacteristic *_pCharSensorVOC = nullptr;

public:
    void setup(const String &name);

    void handle();

    void onConnect();

    void onDisconnect();

    // Wheels/CMD
    void setDrive(const String &value);

    String getDrive() const;

    // Arm
    String getArm() const;

    void setArm(const String &value);

    // Sensor
    // Temperature
    String getSensorTemp() const;

    void setSensorTemp(const String &value);

    // Humidity
    String getSensorHumidity() const;

    void setSensorHumidity(const String &value);

    // Pressure
    String getSensorPressure() const;

    void setSensorPressure(const String &value);

    // Gas Res
    String getSensorIAQ() const;

    void setSensorIAQ(const String &value);

    // Gas Index
    String getSensorCO2() const;

    void setSensorCO2(const String &value);

    // Gas Index
    String getSensorVOC() const;

    void setSensorVOC(const String &value);


    // Get variables for later use
    //-255 ... 255
    int getSpeed0() const { return _speed0; }
    int getSpeed1() const { return _speed1; }
    // Einstellungswert der Stollen
    int getWheel0() const { return _wheel0; }
    int getWheel1() const { return _wheel1; }
    // Einstellungswert der einzelnen Armelemente zwischen 0 bis 180
    // Grenzwerte sind zur Sicherheit festgelegt, da sonst die Motoren durch Dauerlast durchbrennen oder der Arm kaput geht
    int getArm0() const { return _arm0; }
    int getArm1() const { return _arm1; }
    int getArm2() const { return _arm2; }
    int getArm3() const { return _arm3; }
};

extern ESP32ble esp32ble;
