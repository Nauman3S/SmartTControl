#include <AutoPID.h>


#define TEMP_READ_DELAY 800 //can only read digital temp sensor every ~750ms

//pid settings and gains
#define OUTPUT_MIN 0
#define OUTPUT_MAX 255
#define KP .12
#define KI .0003
#define KD 0
const double TimeBase=5000;//pulse width

double temperature, outputVal;
double setPoint=22.13;
bool relayState;



//input/output variables passed by reference, so they are updated automatically
//AutoPID myPID(&temperature, &setPoint, &outputVal, OUTPUT_MIN, OUTPUT_MAX, KP, KI, KD);
AutoPIDRelay myPID(&temperature, &setPoint, &relayState, TimeBase,  KP, KI, KD);

unsigned long lastTempUpdate; //tracks clock time of last temp update

//call repeatedly in loop, only updates after a certain time interval
//returns true if update happened
void setPointConfig(double setPt){
    setPoint=setPt;

}
bool updateTemperature() {
  if ((millis() - lastTempUpdate) > TEMP_READ_DELAY) {
    temperature = getTempValue();
    lastTempUpdate = millis();
    requestTemp(); //request reading for next time
    return true;
  }
  return false;
}//void updateTemperature


void setupPID() {
  
  
  requestTemp();
  while (!updateTemperature()) {} //wait until temp sensor updated

  //if temperature is more than 4 degrees below or above setpoint, OUTPUT will be set to min or max respectively
  myPID.setBangBang(4);
  //set PID update interval to 4000ms
  myPID.setTimeStep(4000);

}//void setup


void loopPID() {
  updateTemperature();
  //setPoint = analogRead(POT_PIN);
  myPID.run(); //call every loop, updates automatically at certain time interval
  //analogWrite(OUTPUT_PIN, outputVal);
  //digitalWrite(LED_PIN, myPID.atSetPoint(1)); //light up LED when we're at setpoint +-1 degree
  changeRelayState(relayState);

}//void loop

