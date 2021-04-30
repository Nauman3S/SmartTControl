
const int Analog_channel_pin= 15;//Potentiometer Pin

int ADC_VALUE = 0;
int voltage_value = 0; 

#include "consts_Headers.h"



void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  setupFreq();
  SetupRelay();
 // setupDS18B20();
  
  setupPID();
  
  
  pinMode(BUILTIN_LED, OUTPUT);


  
   
}

void loop() {
 
  loopPID();
 
  loopZCD();
  
 Serial .print('Frequency : ');
 Serial.println(frequency);


  
}
