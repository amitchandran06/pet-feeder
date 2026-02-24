#include <HX711.h>

extern const int HX711_DT;
extern const int HX711_SCK;
extern const float massConversionFactor; // Default small value
extern int targetMass;
extern int offset;
extern  float massRemaining;
extern int frequency;
extern HX711 scale;

void calibrateScale();

float checkMass(float targetMass);

void initScale();

void setTarget(std::string mealInfo);