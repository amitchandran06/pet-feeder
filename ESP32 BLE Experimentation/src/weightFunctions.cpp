#include <Arduino.h>
#include <weightFunctions.h>
#include <sstream>
#include <motorFunctions.h>
#include <ArduinoJson.h>
const int HX711_DT = 15;
const int HX711_SCK = 16;
int targetMass =0;
int frequency =0;
float massRemaining = 0;
HX711 scale;
const int MAX_MEALS = 10;          // Set a safe maximum number of meals
Meal scheduledMeals[MAX_MEALS];    // Create an array to hold the meals
int totalMeals = 0;                // Keep track of how many meals we actually received
String currentSystemTime = "";     // Global string to hold the system time




float checkMass(float targetMass){
    long raw = scale.read();              // raw ADC value
    float mass = scale.get_units(10);     // averaged reading
    // Convert to mass using conversion factor
    float massRemaining = targetMass - mass;
    Serial.println(mass);
    return massRemaining;
}

void setTarget(std::string mealInfo){
   const char* c_str_data = mealInfo.c_str();

StaticJsonDocument<512> doc;
    DeserializationError error = deserializeJson(doc, mealInfo);

    if (error) {
        Serial.print(F("deserializeJson() failed: "));
        Serial.println(error.f_str());
        return;
    }

    // 3. Save the System Time globally
    // .as<String>() forces it to make a permanent copy
    currentSystemTime = doc["systemTime"].as<String>(); 
    

    // 4. Loop through the meals and save them globally
    JsonArray mealsArray = doc["meals"];
    totalMeals = 0; // Reset count just in case this is called multiple times
    
    for (JsonObject meal : mealsArray) {
        if (totalMeals >= MAX_MEALS) {
            Serial.println("Hit maximum meal limit!");
            break; // Stop if we get more meals than our array can hold
        }
        
        // Save into our global array
        scheduledMeals[totalMeals].qty = meal["qty"];
        scheduledMeals[totalMeals].time = meal["time"].as<String>();
        
        totalMeals++; // Increment our counter
    }
    
    Serial.println("Data parsed and saved globally!");
    Serial.println(scheduledMeals[0].qty);
    Serial.println(scheduledMeals[1].time);


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