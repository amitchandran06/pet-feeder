#include <HX711.h>

extern const int HX711_DT;
extern const int HX711_SCK;
extern int massConversionFactor; // Default small value
extern int targetMass;
extern HX711 scale;

void calibrateScale();

float checkMass(float targetMass);