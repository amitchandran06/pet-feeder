#include <Arduino.h>
#include <NimBLEDevice.h>
#include "bleFunctions.h"   // Use quotes for local files
#include "motorFunctions.h" 

// Implementation of MyCallbacks
void MyCallbacks::onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo& connInfo) {
    std::string value = pCharacteristic->getValue(); 
    if (value.length() > 0) {
        std::string message = "ESP32 Received: " + value;
        motorControlBLE(value.c_str());
        pTxCharacteristic->setValue(message);
        pTxCharacteristic->notify();
    }
}

void MyCallbacks::onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) {
    if(subValue > 0) {
        pTxCharacteristic->setValue("Hi, you have connected!");
        pTxCharacteristic->notify();
    }
}

// Implementation of ServerCallbacks
void ServerCallbacks::onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) {
    deviceConnected = true;
    Serial.printf("Client connected: %s\n", connInfo.getAddress().toString().c_str());
}

void ServerCallbacks::onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) {
    deviceConnected = false;
    Serial.println("Client disconnected - Restarting Advertising");
    NimBLEDevice::startAdvertising();
}