#pragma once
#include <Arduino.h>
#include <NimBLEDevice.h>

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