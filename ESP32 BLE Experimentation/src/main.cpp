// PETFEEDER FIRMWARE - VERSION 1 (Beta) - by Amit

#include <Arduino.h>
#include <NimBLEDevice.h>
#include <AccelStepper.h>
#include <HX711.h>
#include <bleFunctions.h>
#include <motorFunctions.h>
#include <weightFunctions.h>
#include <time.h>
#include <timeKeep.h>

unsigned long lastWeightCheck = 0;
const long weightInterval = 1300; // Check weight every 1300ms (corresponds to roughly 120 degrees of motor movement)
unsigned long lastTimeCheck =0;
const long timeInterval = 1000*10; // Check time every 10s ( this level of accuracy is enough)
NimBLECharacteristic *pTxCharacteristic = nullptr;
NimBLECharacteristic *pRxCharacteristic = nullptr;
volatile bool deviceConnected = false;


void setup() {
    Serial.begin(115200);
    Serial.println("Starting BLE Work!");
    pinMode(stepPin,OUTPUT); // Set the driver pins to outputs
    pinMode(dirPin , OUTPUT);
    pinMode(enPin , OUTPUT);
    motorOn = false; // Default to the motor being off
    motor.setMaxSpeed(50);
    motor.setAcceleration(500);

    // BLE SETUP
    // 1.  Start by Initialising the Device
    NimBLEDevice::init("S40 Pet Feeder Device");
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
    calibrateScale();  
}



void loop() {
   // Creating a currentTime stamp
    unsigned long currentMillis = millis();

    //Here we check if a set interval of time has passed before updating system time (to reduce system load)
    if(currentMillis - lastTimeCheck >= timeInterval){
        updateGlobalTime();
        lastTimeCheck = currentMillis; 
    }

    bool runningState = false; // Start the loop by setting motor flag false 

    // This loop iterates through the scheduledMeal struct (from JSON) and searches if the time matches 
    for(int i = 0; i <= totalMeals; i++){
      // the first check is to see if the time is correct for the feeding scheulde (to the minute)
        if(String(globalTimeStr) == scheduledMeals[i].time){

         // If the time conidtion is met, we check what the mass of the scale is
            if(currentMillis - lastWeightCheck >= weightInterval) {
                massRemaining = checkMass(int(scheduledMeals[i].qty));
                lastWeightCheck = currentMillis;
            }
            // Only if there is a positive remaining mass do we set the running state to ON  
            if(massRemaining > 0){
               runningState = true; 
            }
        }
    }

    // Set motorOn to whatever the runningState is
    motorOn = runningState;

    // This calls the function in motorFunctions.cpp to actually run the motor
    motorControlBLE();
}