#include <OneWire.h>
#include <DallasTemperature.h>

#define ONE_WIRE_BUS 2

// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature. 
DallasTemperature sensors(&oneWire);


void SetupRelay(){
    pinMode(4, OUTPUT);
}

void changeRelayState(int state){
    if(state==0){
        digitalWrite(4,LOW);
    }
    else if(state==1){
        digitalWrite(4,HIGH);
    }

}



void setupDS18B20() {
 sensors.begin();
}
int requestTemp(){
  sensors.requestTemperatures();
}
float getTempValue() {
  float tempC = sensors.getTempCByIndex(0);

  // Check if reading was successful
  if(tempC != DEVICE_DISCONNECTED_C) 
  {
    Serial.print("Temperature for the device 1 (index 0) is: ");
    Serial.println(tempC);
    return(tempC);
  } 
  
}
String getTempStr(){
    String t=String(getTempValue());
    requestTemp();
    return(t);
}
int compareValues(float remoteV, float sensorV){
    if(remoteV<sensorV || remoteV==sensorV){
      return 0;//do nothing
    }
    else if(remoteV>sensorV){
        return 1;//start heating; turn on the relay
    }

}