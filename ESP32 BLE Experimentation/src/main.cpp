// PETFEEDER FIRMWARE - VERSION 0.2 BETA (BLE TESTS) - by Amit

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <AccelStepper.h>
#include <HX711.h>
#include <bleFunctions.h>
#include <motorFunctions.h>
#include <weightFunctions.h>

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

NimBLECharacteristic *pTxCharacteristic = nullptr;
NimBLECharacteristic *pRxCharacteristic = nullptr;
volatile bool deviceConnected = false;


void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Work!");
    pinMode(stepPin,OUTPUT);
    pinMode(dirPin , OUTPUT);
    motor.setMaxSpeed(500);
    motor.setAcceleration(1000);

    // BLE SETUP
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
    
        // HX711/LOAD CELL/SCALE SETUP
    Serial.println("Initializing HX711...");
    scale.begin(HX711_DT, HX711_SCK);
    // Optional: set gain (128 is default for channel A)
    scale.set_gain(128);
    // Tare (zero the scale)
    Serial.println("Taring...");
    scale.tare(10);  // average over 10 readings
    Serial.println("HX711 ready.");
}



void loop() {
    // Converted HX711 Input
    float mass = checkMass(targetMass);
    

    if(motorOn) {
        if (motor.distanceToGo() < 500) {
            motor.move(10000); // Add a large chunk of steps
        }
    }
}