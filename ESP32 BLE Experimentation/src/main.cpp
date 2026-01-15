// PETFEEDER FIRMWARE - VERSION 0.2 BETA (BLE TESTS) - by Amit

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <AccelStepper.h>
#include <HX711.h>
#include <bleFunctions.h>
#include <motorFunctions.h>
#include <weightFunctions.h>



NimBLECharacteristic *pTxCharacteristic = nullptr;
NimBLECharacteristic *pRxCharacteristic = nullptr;
volatile bool deviceConnected = false;


void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Work!");
    pinMode(stepPin,OUTPUT);
    pinMode(dirPin , OUTPUT);
    motorOn = false;
    motor.setMaxSpeed(500);
    motor.setAcceleration(1000);

    // BLE SETUP
    // 1.  Start by Initialising the Device
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
    
    //Initialise HX711 scale
    initScale();  
}



void loop() {
    // Converted HX711 Input
    float massRemaining = checkMass(targetMass);
    
    if (massRemaining > 0) {
        motorOn = true;
    }
    else{
        motorOn = false;
        mealEnd(frequency);
    }
motorControlBLE();
  
}