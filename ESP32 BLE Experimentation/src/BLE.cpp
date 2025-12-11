// PETFEEDER FIRMWARE - VERSION 0.1 BETA (BLE TESTS) - by Amit

#include <Arduino.h>
#include <NimBLEDevice.h>

// -------------------------------------------------------------------------
// UUID is up to the discretion of the developer or installer. UUID must be the same on both the mobile client as well the MCU
// to ensure that Bluetooth device filtering works effectively 
// -------------------------------------------------------------------------
#define SERVICE_UUID           "6E400001-B5A3-F393-E0A9-E50E24DCCA9E" 
#define RX_CHARACTERISTIC_UUID "6E400002-B5A3-F393-E0A9-E50E24DCCA9E" // App writes here
#define TX_CHARACTERISTIC_UUID "6E400003-B5A3-F393-E0A9-E50E24DCCA9E" // ESP32 notifies here

// These are alternative UUIDs to ensure filtering within the Android Client (ONLY USE ONE SET OF UUIDS PLS)
/*
#define SERVICE_UUID           "A495FF20-C5B5-4B44-B512-1370F02D74DE" 
#define RX_CHARACTERISTIC_UUID "A495FF20-C5B5-4B44-B512-1370F02D74DE" 
#define TX_CHARACTERISTIC_UUID "A495FF20-C5B5-4B44-B512-1370F02D74DE"
*/

// Defining the BLE containers (Characteristics) for RX and TX as NimBLE characteristic objects
NimBLECharacteristic *pTxCharacteristic;
NimBLECharacteristic *pRxCharacteristic;
// Current device conencted status
volatile bool deviceConnected = false;
const int ledPin = 14; // Pin to attach LED +Ve pin to 


// This function toggles the LED On/Off depending on the inputs
void ledToggle(std::string toggleStatus){
    String toggle = toggleStatus.c_str(); // Converting to standard string type
    Serial.println(toggle);

    // These conditions compare the input to ON / OFF to toggle (including the new line spacing)
    if(toggle == "ON\r\n"){
    digitalWrite(ledPin,HIGH);
    }
    if(toggle == "OFF\r\n"){
    digitalWrite(ledPin,LOW);
    }
    else{
    }
}

// 1. DATA RECEIVING CALLBACKS (Phone -> ESP32)
class MyCallbacks:
 public NimBLECharacteristicCallbacks {
    
    // Here we override the exisitng onWrite method, in this case to print the recieved data to serial
    void onWrite(NimBLECharacteristic *pCharacteristic, NimBLEConnInfo& connInfo) override {
        std::string value = pCharacteristic->getValue(); 
        // Checking for data and "echoing" back to the phone to confirm the message arrived
        if (value.length() > 0) {
        std::string  message = "ESP32 Recieved:  " + value;
        // Calling the LED toggle and inputting the message in
        ledToggle(value.c_str());
        // Echo of input
        pTxCharacteristic->setValue(message);
        pTxCharacteristic->notify();
        }
    }
    //This function is called / overwrites the exisiting function for when a device is subscribed to the bluetooth notifications (for first time connection)
    void onSubscribe(NimBLECharacteristic* pCharacteristic, NimBLEConnInfo& connInfo, uint16_t subValue) override {

        if(subValue>0){
         // Transmitts a confirmation message
        pTxCharacteristic->setValue("Hi, you have connected to the Pet Feeder's ESP32!!");
        pTxCharacteristic->notify();
        }
     }
           
        
    
};

// 2. CONNECTION CALLBACKS (Connection Status)
class ServerCallbacks:
 public NimBLEServerCallbacks {
    
    // Overriding the onConnect function to print the address of the client
    void onConnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo) override {
        deviceConnected = true;
        Serial.printf("Client connected: %s\n", connInfo.getAddress().toString().c_str());
    }

    //Similar procedure for disconnection
    void onDisconnect(NimBLEServer* pServer, NimBLEConnInfo& connInfo, int reason) override {
        deviceConnected = false;
        Serial.println("Client disconnected - Restarting Advertising");
        NimBLEDevice::startAdvertising();
    }
};



void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Work!");
    pinMode(ledPin,OUTPUT);

    // 1.  Stat by Initialising the Device
    NimBLEDevice::init("ESP32-UART-Device");
    NimBLEDevice::setMTU(517);
    // 2. Creating the server
    NimBLEServer *pServer = NimBLEDevice::createServer();
       // Setting the callbacks to the one we created for server connect / DC events
       pServer->setCallbacks(new ServerCallbacks());

    // 3. Create Service 
    NimBLEService *pService = pServer->createService(SERVICE_UUID);

    // 4. Create RX Characteristic (Messages from Android are recieved at this point)
    pRxCharacteristic = pService->createCharacteristic(RX_CHARACTERISTIC_UUID, NIMBLE_PROPERTY::WRITE | NIMBLE_PROPERTY::WRITE_NR); // "Write No Response" is faster
       // Setting a new callback upon recieving             
       pRxCharacteristic->setCallbacks(new MyCallbacks());

    // 5. Create TX Characteristic (ESP32 sends updates to this).
    pTxCharacteristic = pService->createCharacteristic(TX_CHARACTERISTIC_UUID,NIMBLE_PROPERTY::NOTIFY);
       //This callback is for the onSubscribe method
       pTxCharacteristic->setCallbacks(new MyCallbacks());
    // 6. Start Service
    pService->start();

    // 7. Now that the service has started and the callbacks are assigned to their relative points, we can begin advertising
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
    //Loop is empty as all code is run on a callback basis (waiting for an input from the client), the server only RESPONDS
}