#include <HX711.h>

extern const int HX711_DT;
extern const int HX711_SCK;
extern int massConversionFactor; // Default small value
extern int targetMass;
extern int offset;
 int amount;
  int freq;
 char comma;
extern HX711 scale;

void calibrateScale();

float checkMass(float targetMass);

void initScale();

void setTarget(std::string mealInfo);