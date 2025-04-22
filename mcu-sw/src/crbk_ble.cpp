#include <crbk_ble.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2904.h>
#include <BLE2902.h>
#include <BLE2901.h>

CrbkRCCarBLE crbkRCCarBLE;

class CmdCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
      crbkRCCarBLE.setCmd(pCharacteristic->getValue());    
  }
};

class StateCallbacks : public BLECharacteristicCallbacks {
  void onRead(BLECharacteristic *pCharacteristic) {    
    pCharacteristic->setValue(crbkRCCarBLE.getCmd());
    pCharacteristic->notify();
  }
};

class ServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    crbkRCCarBLE.onConnect();
  };

  void onDisconnect(BLEServer *pServer) {
    crbkRCCarBLE.onDisconnect();
  }
};

String CrbkRCCarBLE::getCmd() const { 
  return "Speed: "+String(_speed)+", Direction: "+String(_direction)+"("+String(getDriveMode())+")"; 
}

void CrbkRCCarBLE::setCmd(String value) {
  String cmd = value;

  // prüfen auf kombinierte anweisung: // `${direction}:${speed}`
  int t=cmd.indexOf(":");
  if(t>0 && t < value.length() ) {
    _direction = value.substring(0,t).toInt();
    _speed = value.substring(t+1,value.length()).toInt();
    
    
    if(_speed>255) _speed=255;
    if(_speed<-255) _speed=-255;
    if(_direction>90) _direction=90;
    if(_direction<-90) _direction=-90;

  } else {
    if(cmd=="left") {
      _direction-=1;
      if(_direction<-90) _direction=-90;
    } else if(cmd=="right") {
      _direction+=1;
      if(_direction>90) _direction=90;
    } else if(cmd=="straight") {
      _direction=0;
    } else if(cmd=="up") {
      _speed+=10;
      if(_speed>255) _speed=255;
    } else if(cmd=="down") {
      _speed-=10;
      if(_speed<-255) _speed=-255;
    } else if(cmd=="led1-on") {
      _led1=true;
    } else if(cmd=="led1-off") {
      _led1=false;
    } else if(cmd=="led2-on") {
      _led2=true;
    } else if(cmd=="led2-off") {
      _led2=false;
    } else {
      // default stop!
      _speed=0;
    }
  }

  if(_pStateCharacteristic) {
    _pStateCharacteristic->setValue(getCmd());
    _pStateCharacteristic->notify();
  }
}

int CrbkRCCarBLE::getDriveMode() const { 
  if(_speed==0)
    return STOPPED;
    
  if(_speed > 0) {
    if(_direction<0) return LEFTFORWARD;
    else if(_direction>0) return RIGHTFORWARD;
    return FORWARD;
  } else {
    if(_direction<0) return LEFTBACKWARD;
    else if(_direction>0) return RIGHTBACKWARD;
    return BACKWARD;
  }
}


void CrbkRCCarBLE::onConnect() {
  _connected = true;
}

void CrbkRCCarBLE::onDisconnect() {
  _connected = false;
}

void CrbkRCCarBLE::setup(String name) {
  // Create the BLE Device
  this->_name = name;
  BLEDevice::init(name);

  // Create the BLE Server
  _pServer = BLEDevice::createServer();
  _pServer->setCallbacks(new ServerCallbacks());

  // Create the BLE Service
  BLEService *pService = _pServer->createService(SERVICE_UUID);

  BLECharacteristic *pCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_CMD,
    BLECharacteristic::PROPERTY_WRITE | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE
  );

  // Creates BLE Descriptor 0x2902: Client Characteristic Configuration Descriptor (CCCD)
  pCharacteristic->addDescriptor(new BLE2902());
  pCharacteristic->setCallbacks(new CmdCallbacks());

  {
    // Adds also the Characteristic User Description - 0x2901 descriptor
    BLE2901 *descriptor_2901 = new BLE2901();
    descriptor_2901->setDescription("drive_control");
    descriptor_2901->setAccessPermissions(ESP_GATT_PERM_READ);  // enforce read only - default is Read|Write
    pCharacteristic->addDescriptor(descriptor_2901);
    // Adds also the Characteristic Type Description - 0x2904 descriptor
    BLE2904 *descriptor_2904 = new BLE2904();
    descriptor_2904->setFormat(BLE2904::FORMAT_UTF8);
    pCharacteristic->addDescriptor(descriptor_2904);
  }
  // Create a BLE State Characteristic
  _pStateCharacteristic = pService->createCharacteristic(
    CHARACTERISTIC_STATE,
    BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_NOTIFY | BLECharacteristic::PROPERTY_INDICATE
  );
  _pStateCharacteristic->setCallbacks(new StateCallbacks());

  {
    // Adds also the Characteristic User Description - 0x2901 descriptor
    BLE2901 *descriptor_2901 = new BLE2901();
    descriptor_2901->setDescription("drive_state");
    descriptor_2901->setAccessPermissions(ESP_GATT_PERM_READ);  // enforce read only - default is Read|Write
    _pStateCharacteristic->addDescriptor(descriptor_2901);
    // Creates BLE Descriptor 0x2902: Client Characteristic Configuration Descriptor (CCCD)
    _pStateCharacteristic->addDescriptor(new BLE2902());
    // Adds also the Characteristic Type Description - 0x2904 descriptor
    BLE2904 *descriptor_2904 = new BLE2904();
    descriptor_2904->setFormat(BLE2904::FORMAT_UTF8);
    _pStateCharacteristic->addDescriptor(descriptor_2904);
  }
  
  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  // pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    pAdvertising->setMinPreferred(0x06);  // functions that help with iPhone connections issue
  pAdvertising->setMinPreferred(0x12);
  BLEDevice::startAdvertising();
}

void CrbkRCCarBLE::handle() {
  // disconnecting
  if (!_connected && _lastConnectionState) {
    delay(500);                     // give the bluetooth stack the chance to get things ready
    BLEDevice::startAdvertising();  // restart advertising
    _lastConnectionState = _connected;
  }
  // connecting
  if (_connected && !_lastConnectionState) {
    _lastConnectionState = _connected;
  }
}
