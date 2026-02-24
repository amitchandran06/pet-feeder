#include <Arduino.h>
#include <weightFunctions.h>
#include <sstream>
#include <motorFunctions.h>
const int HX711_DT = 15;
const int HX711_SCK = 16;
int targetMass =0;
int frequency =0;
float massRemaining = 0;
HX711 scale;

float checkMass(float targetMass){
    long raw = scale.read();              // raw ADC value
    float mass = scale.get_units(10);     // averaged reading
    // Convert to mass using conversion factor
    float massRemaining = targetMass - mass;
    Serial.println(mass);
    return massRemaining;
}

void setTarget(std::string mealInfo){
    int amount = 0;
    int freq = 0;
    char comma = ',';
    motorOn = false;
    std::stringstream ss(mealInfo);
    ss >> amount >> comma >> freq;
    targetMass = amount;
    massRemaining = float(amount);
    frequency = freq;
    Serial.println(amount);
}

void calibrateScale()
{
// Tare (zero the scale)
    Serial.println("Taring...");
    scale.tare(20);  // average over 20 readings
    Serial.println("HX711 ready.");
}

void initScale(){
    // HX711/LOAD CELL/SCALE SETUP
    Serial.println("Initializing HX711...");
    scale.begin(HX711_DT, HX711_SCK);
    // Optional: set gain (128 is default for channel A)
    scale.set_gain(128);
    scale.set_scale(387.7);
    
}