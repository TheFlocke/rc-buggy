// Ändere diese Werte >>>>
// See the following for generating UUIDs: https://www.uuidgenerator.net/
#define SERVICE_UUID         "5eaf1079-e806-47a9-a1ec-d815bea94805"
#define CHARACTERISTIC_CMD   "7cb6bbe0-f35e-4a34-a8e2-6731102e12e3"
#define CHARACTERISTIC_STATE "bd6fbfde-385d-480f-b5eb-64d60cc7be9a"

#define VERSION              "0.0.1"
// Ändere diese Werte <<<<<

#include <BLEDevice.h>
#include <BLEServer.h>


class CrbkRCCarBLE {

  String _name;
  bool   _connected = false;
  bool   _lastConnectionState = false;
  int    _speed=0;
  int    _direction=0;
  bool   _led1=false;
  bool   _led2=false;
  
  BLEServer *_pServer = NULL;
  BLECharacteristic *_pStateCharacteristic = NULL;

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

    bool getLED1() const { return _led1; }
    bool getLED2() const { return _led2; }

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

extern CrbkRCCarBLE crbkRCCarBLE;
