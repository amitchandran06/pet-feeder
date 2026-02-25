
#include "motorFunctions.h"
#include <Arduino.h>
#include <NimBLEDevice.h>
#include <AccelStepper.h>
#include <HX711.h>
#include <sstream>
#include <weightFunctions.h>

bool motorOn = false;
const int stepPin = 14; // Pin to attach to STEP of the driver
const int dirPin = 13; // Pin to attach to DIR 
AccelStepper motor(1, stepPin , dirPin);

// This function toggles the Motor on and Off via bluetooth
void motorControlBLE(){
  if(motorOn) {
        if(motor.distanceToGo()<500){
            motor.move(10000); // Add a large chunk of steps
        }
    }

    else {
        motor.stop();
    }
    motor.run();
}

void mealEnd(){
   motor.stop() ;
   while(motor.distanceToGo() > 0){
    motor.run();
   } 

}