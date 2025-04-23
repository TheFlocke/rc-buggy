#include "../lib/ble.h"


void blue_setup() {
    NimBLEDevice::init("rc-rover");

    NimBLEServer *pServer = NimBLEDevice::createServer();
    NimBLEService *pService = pServer->createService("ABCD");
    NimBLECharacteristic *pCharacteristic = pService->createCharacteristic("1234");

    pService->start();
    pCharacteristic->setValue("Hello BLE");

    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising(); // create advertising instance
    pAdvertising->addServiceUUID("ABCD"); // advertise the UUID of our service
    pAdvertising->setName("rc-rover"); // advertise the device name
    pAdvertising->start(); // start advertising
}

void loop() {

}
