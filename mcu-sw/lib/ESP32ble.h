#include "NimBLEDevice.h"
#include "../lib/Servo.h"
#include "../lib/Stepper.h"

// See the following for generating UUIDs: https://www.uuidgenerator.net/
// CMD Service
#define UUID_SERVICE_CMD                        "5eaf1079-e806-47a9-a1ec-d815bea94805"
#define UUID_CHAR_CMD                           "7cb6bbe0-f35e-4a34-a8e2-6731102e12e3"
#define UUID_CHAR_CMD_STATE                     "bd6fbfde-385d-480f-b5eb-64d60cc7be9a"
// Sensor Service
#define UUID_SERVICE_SENSOR                     "8da7a992-e263-4b78-abf2-bdb94808895c"
#define UUID_CHAR_SENSOR_TEMP                   "24c53354-de00-42ac-926c-31f805e5d2f5"
#define UUID_CHAR_SENSOR_PRESSURE               "545343fb-93a0-4415-9e2b-6a4c8e2835c4"
#define UUID_CHAR_SENSOR_HUMIDITY               "618c8e95-e436-4a86-9d7d-17c3db9992d0"
#define UUID_CHAR_SENSOR_IAQ                    "aa4ce7cf-fff0-4d54-b4ec-fb24920b35c1"
#define UUID_CHAR_SENSOR_CO2                    "3b9e4e45-42f2-4892-8d63-35f4a4bc8093"
#define UUID_CHAR_SENSOR_VOC                    "988b016d-91a0-4830-ae55-650ef2bb9c8d"

// Variables (with latest) values that could be used in other functions

extern int speed0;
extern int speed1;
extern int wheel0;
extern int wheel1;
extern int arm0;
extern int arm1;
extern int arm2;
extern int arm3;

// array for outside (and inside) use that corresponds to ids 01 -> speed0
extern int *vars[8];

class ESP32ble {
    // everything for BLE connection
    String _name;
    bool _connected = false;
    bool _lastConnectionState = false;
    // for get command so it corresponds to the latest value
    int _latestID = -1;

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
    NimBLECharacteristic *_pCharCmdState = nullptr;
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
    void setCMD(const String &value);

    String getCMD() const;


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
    int getSpeed0() const { return speed0; }
    int getSpeed1() const { return speed1; }
    // Einstellungswert der Stollen
    int getWheel0() const { return wheel0; }
    int getWheel1() const { return wheel1; }
    // Einstellungswert der einzelnen Armelemente zwischen 0 bis 180
    // Grenzwerte sind zur Sicherheit festgelegt, da sonst die Motoren durch Dauerlast durchbrennen oder der Arm kaput geht
    int getArm0() const { return arm0; }
    int getArm1() const { return arm1; }
    int getArm2() const { return arm2; }
    int getArm3() const { return arm3; }
};

extern ESP32ble esp32ble;
