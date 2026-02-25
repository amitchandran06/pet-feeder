#ifndef WEIGHT_FUNCTIONS_H
#define WEIGHT_FUNCTIONS_H

#include <HX711.h>

extern const int HX711_DT;
extern const int HX711_SCK;
extern const float massConversionFactor; // Default small value
extern int targetMass;
extern int offset;
extern  float massRemaining;
extern int frequency;
extern HX711 scale;

// 1. Define a "blueprint" for a meal
struct Meal {
    int qty;
    String time; // Using 'String' here safely copies the text so it isn't lost
};
extern const int MAX_MEALS;
extern Meal scheduledMeals[10];
extern int totalMeals;
extern String currentSystemTime;

void calibrateScale();

float checkMass(float targetMass);

void initScale();

void setTarget(std::string mealInfo);

#endif // WEIGHT_FUNCTIONS_H