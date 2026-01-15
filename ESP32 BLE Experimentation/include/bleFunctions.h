#pragma once
#include <Arduino.h>
#include <NimBLEDevice.h>

// -------------------------------------------------------------------------
// UUID is up to the discretion of the developer or installer. UUID must be the same on both the mobile client as well the MCU
// to ensure that Bluetooth device filtering works effectively 
// -------------------------------------------------------------------------
/*
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" 
#define RX_CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" 
#define TX_CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E"
*/
// These are alternative UUIDs to ensure filtering within the Android Client (ONLY USE ONE SET OF UUIDS PLS)

// 1. UUID DEFINITIONS
// Use constexpr for better type safety, or #define if you prefer.
#define SERVICE_UUID           "A495FF20-C5B5-4B44-B512-1370F02D74DE" 
#define RX_CHARACTERISTIC_UUID "A495FF20-C5B5-4B44-B512-1370F02D74DE" 
#define TX_CHARACTERISTIC_UUID "A495FF20-C5B5-4B44-B512-1370F02D74DE"

// 2. SHARED VARIABLES (Externs)
extern NimBLECharacteristic *pTxCharacteristic;
extern volatile bool deviceConnected;

// 3. CALLBACK CLASSES
class MyCallbacks : public NimBLECharacteristicCallbacks {
public:
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo& connInfo) override;
    void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) override;
};

class ServerCallbacks : public NimBLEServerCallbacks {
public:
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override;
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override;
};