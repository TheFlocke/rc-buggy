// See the following for generating UUIDs: https://www.uuidgenerator.net/
#define SERVICE_UUID         "5eaf1079-e806-47a9-a1ec-d815bea94805"
#define CHARACTERISTIC_CMD   "7cb6bbe0-f35e-4a34-a8e2-6731102e12e3"
#define CHARACTERISTIC_STATE "bd6fbfde-385d-480f-b5eb-64d60cc7be9a"

#define VERSION         "0.0.1"

#include "NimBLEDevice.h"

class ble {

    String _name;
    bool _connected = false;
    bool _lastConnected = false;
    int _speed = 0;
    int _direction = 0;
};