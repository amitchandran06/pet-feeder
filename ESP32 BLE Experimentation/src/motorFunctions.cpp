
#include "motorFunctions.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <AccelStepper.h>
#include <HX711.h>

bool motorOn = false;
const int stepPin = 14; // Pin to attach to STEP of the driver
const int dirPin = 13; // Pin to attach to DIR 

AccelStepper motor(1, stepPin , dirPin);

// This function toggles the Motor on and Off via bluetooth
void motorControlBLE(std::string toggleStatus){
    String toggle = toggleStatus.c_str(); // Converting to standard string type
    Serial.println(toggle);

    // These conditions compare the input to ON / OFF to toggle (including the new line spacing)
    if(toggle == "ON\r\n" || toggle == "ON"){
    motorOn = true;
    }
    if(toggle == "OFF\r\n" || toggle == "OFF"){
    motorOn = false;
    motor.stop();
    }
    else{
    }
}