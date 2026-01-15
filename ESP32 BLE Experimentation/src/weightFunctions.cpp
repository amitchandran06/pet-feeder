#include <Arduino.h>
#include <weightFunctions.h>
#include <sstream>
#include <motorFunctions.h>
const int HX711_DT = 15;
const int HX711_SCK = 16;
const float massConversionFactor = 10^-6; // Default small value
const int offset = 0;
int targetMass =0;
HX711 scale;

float checkMass(float targetMass){
    long raw = scale.read();              // raw ADC value
    float units = scale.get_units(5);     // averaged reading

    // Convert to mass using conversion factor
    Serial.println(raw);
    float mass = (raw-offset)*massConversionFactor;
    float massRemaining = targetMass - mass;
    return massRemaining;
}

void setTarget(std::string mealInfo){
    motorOn = false;
    String mealInfo = mealInfo.c_str(); // Converting to standard string type
    std::stringstream ss(mealInfo);
    ss >> amount >> comma >> freq;
    targetMass = amount;
}

void calibrateScale()
{

}

void initScale(){
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