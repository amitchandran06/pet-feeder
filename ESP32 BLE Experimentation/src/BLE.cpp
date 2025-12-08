#include <Arduino.h>
#include <NimBLEDevice.h>

// -------------------------------------------------------------------------
// STANDARD NORDIC UART SERVICE (NUS) UUIDs
// Using these allows standard "Serial Bluetooth Terminal" apps to work.
// -------------------------------------------------------------------------
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E"
#define RX_CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" // App writes here
#define TX_CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" // ESP32 notifies here

NimBLECharacteristic *pTxCharacteristic;
NimBLECharacteristic *pRxCharacteristic;
volatile bool deviceConnected = false;

// 1. DATA RECEIVING CALLBACKS (Phone -> ESP32)
class MyCallbacks: public NimBLECharacteristicCallbacks {
    
    // NIMBLE V2 SIGNATURE: Includes 'NimBLEConnInfo&'
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo& connInfo) override {
        std::string value = pCharacteristic->getValue(); 
        
        if (value.length() > 0) {
            Serial.print("Received from App: ");
            Serial.println(value.c_str());

            // Example: Control an LED if specific text arrives
            // if (value == "ON") digitalWrite(2, HIGH);
        }
    }
};

// 2. CONNECTION CALLBACKS (Connection Status)
class ServerCallbacks: public NimBLEServerCallbacks {
    
    // NIMBLE V2 SIGNATURE
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        deviceConnected = true;
        Serial.printf("Client connected: %s\n", connInfo.getAddress().toString().c_str());
    };

    // NIMBLE V2 SIGNATURE
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        deviceConnected = false;
        Serial.println("Client disconnected - Restarting Advertising");
        NimBLEDevice::startAdvertising();
    }
};

void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Work!");

    // 1. Initialize Device
    NimBLEDevice::init("ESP32-UART-Device");
    
    // 2. Create Server
    NimBLEServer *pServer = NimBLEDevice::createServer();
    pServer->setCallbacks(new ServerCallbacks());

    // 3. Create Service
    NimBLEService *pService = pServer->createService(SERVICE_UUID);

    // 4. Create RX Characteristic (Phone writes to this)
    pRxCharacteristic = pService->createCharacteristic(
                           RX_CHARACTERISTIC_UUID,
                           NIMBLE_PROPERTY::WRITE | 
                           NIMBLE_PROPERTY::WRITE_NR // "Write No Response" is faster
                       );
    pRxCharacteristic->setCallbacks(new MyCallbacks());

    // 5. Create TX Characteristic (ESP32 sends updates to this)
    pTxCharacteristic = pService->createCharacteristic(
                           TX_CHARACTERISTIC_UUID,
                           NIMBLE_PROPERTY::NOTIFY
                       );

    // 6. Start Service
    pService->start();

    // 7. Setup Advertising
    NimBLEAdvertising *pAdvertising = NimBLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    
    // V2: Set Scan Response manually to ensure name is visible
    NimBLEAdvertisementData scanResponseData;
    scanResponseData.setName("ESP32-UART-Device"); 
    pAdvertising->setScanResponseData(scanResponseData);

    pAdvertising->start();
    
    Serial.println("Advertising Started... Waiting for connection.");
}

void loop() {
    if (deviceConnected) {
        // Example: Send data to the phone every 2 seconds
        std::string message = "Hello from ESP32: " + std::to_string(millis()/1000) + "\n";
        
        pTxCharacteristic->setValue(message);
        pTxCharacteristic->notify();
        
        Serial.print("Sent: ");
        Serial.println(message.c_str());
        
        delay(2000);
    } else {
        // Small delay to prevent watchdog crashing when idle
        delay(500); 
    }
}