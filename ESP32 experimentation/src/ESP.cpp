#include <Arduino.h>

// put function declarations here:
int myFunction(int, int);
void clearBuffer();
void setup() {
Serial.begin(9600);
}

void loop() {
 
   
    
    if(Serial.available()>0){
    String input = Serial.readString();
    Serial.print("You inputted  -->  "); 
    Serial.println( input);
    clearBuffer();
  }

  
}

void clearBuffer() {
  while(Serial.available() >0) {
    Serial.read();
  }
}