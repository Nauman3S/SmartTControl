

#define TEMP_READ_DELAY 755 //can only read digital temp sensor every ~750ms

unsigned long lastTempUpdate; //tracks clock time of last temp update

double temperature, outputVal;
double setPoint = 22.13;
const int aihMargin = 1; //+-1
const int aihThresh = setPoint;
bool relayState;

double aihVal = 0;
bool aihHystOn = 0;

void setPointConfig(double setPt)
{
  setPoint = setPt;
}

double getSetPoint()
{
  return setPoint;
}
bool updateTemperature()
{
  if ((millis() - lastTempUpdate) > TEMP_READ_DELAY)
  {
    temperature = getTempValue();
    lastTempUpdate = millis();
    requestTemp(); //request reading for next time
    return true;
  }
  return false;
} //void updateTemperature

void setupHyst()
{

  requestTemp();
  while (!updateTemperature())
  {
  } //wait until temp sensor updated

} //void setup

bool hystThresh()
{ /* function hystThresh */
  //Perform hysteresis on sensor readings

  if (aihVal >= (aihThresh + aihMargin))
  {
    aihHystOn = true;
  }
  if (aihVal <= (aihThresh - aihMargin))
  {
    aihHystOn = false;
  }

  // Serial.print(F("aih hist : ")); Serial.println(aihHystOn);
  return aihHystOn;
}
void loopHyst()
{
  updateTemperature();
  aihVal = temperature;
  changeRelayState(hystThresh());

} //void loop
