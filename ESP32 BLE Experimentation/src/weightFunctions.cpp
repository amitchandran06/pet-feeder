#include <Arduino.h>
#include <weightFunctions.h>

const int HX711_DT = 15;
const int HX711_SCK = 16;
int massConversionFactor = 10^-6; // Default small value
int targetMass = 15;
HX711 scale;

float checkMass(float targetMass){
    long raw = scale.read();              // raw ADC value
    float units = scale.get_units(5);     // averaged reading

    // Convert to mass using conversion factor
    Serial.println(raw);
    return raw*(10^-6);
}

void calibrateScale()
{

}
