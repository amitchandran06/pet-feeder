#pragma once
#include  <Arduino.h>
#include <AccelStepper.h>
extern bool motorOn;
extern const int stepPin; // Pin to attach to STEP of the driver
extern const int dirPin; // Pin to attach to DIR 
extern const int enPin;
extern AccelStepper motor;


void motorControlBLE();
void mealEnd();