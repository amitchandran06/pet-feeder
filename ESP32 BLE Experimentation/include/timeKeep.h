#include <time.h>
#include <Arduino.h>
#include <weightFunctions.h>

 extern char globalTimeStr[6]; 

void updateGlobalTime(); // Function to refresh the string

void syncTime(String timestamp);