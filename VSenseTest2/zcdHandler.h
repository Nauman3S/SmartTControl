
#define USING_INTERRUPTS 1
volatile int interruptCounter;
int totalInterruptCounter;
 
#if USING_INTERRUPTS
hw_timer_t * timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void timerRestartN(hw_timer_t *timerT) {
    // timerT->dev->config.enable = 0;
    // timerT->dev->reload = 1;
    // timerT->dev->config.enable = 1;
    timerAlarmWrite(timer, 1000000, true); //1 000 000 microseconds, is equal to 1 second.
      timerWrite(timer, 0);
      timerAlarmEnable(timer);

}
 
void IRAM_ATTR onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
 
}
#endif


uint16_t tmr1 = 0;
uint16_t tmr2 = 0;
float period, frequency,frequency2, period2;

const byte interruptPin = 25;
const byte interruptPin2 = 26;
volatile int OpinterruptCounter = 0;
volatile int OpinterruptCounter2 = 0;
int OpnumberOfInterrupts = 0;
int OpnumberOfInterrupts2 = 0;

#if USING_INTERRUPTS
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
portMUX_TYPE mux2 = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR OphandleInterrupt() {
  portENTER_CRITICAL_ISR(&mux);
  OpinterruptCounter++;
  tmr1=OpinterruptCounter;
  portEXIT_CRITICAL_ISR(&mux);
  timerRestartN(timer);
}
void IRAM_ATTR OphandleInterrupt2() {
  portENTER_CRITICAL_ISR(&mux2);
  OpinterruptCounter2++;
  tmr2=OpinterruptCounter2;
  portEXIT_CRITICAL_ISR(&mux2);
  //timerRestartN(timer);
}
#endif
void setupFreq(void) {
 
 

  
  pinMode(interruptPin, INPUT_PULLUP);
  pinMode(interruptPin2, INPUT_PULLUP);
  #if USING_INTERRUPTS
  attachInterrupt(digitalPinToInterrupt(interruptPin), OphandleInterrupt, FALLING);
  attachInterrupt(digitalPinToInterrupt(interruptPin2), OphandleInterrupt2, RISING);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true); //1 000 000 microseconds, is equal to 1 second.
  timerWrite(timer, 0);
  timerAlarmEnable(timer);
  #endif
}
 

float getPeriod(int n){
  if (n==1){
  return frequency;
  }
  else{
    return frequency2;
  }
}
 
float getFrequency(int n){
  if (n==1){
  return period;
  }
  else{
    return period2;
  }
}
void loopZCD() {
 #if USING_INTERRUPTS
  // save current Timer1 value
  if (interruptCounter > 0) {
    
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
 
    totalInterruptCounter++;
 
 //   Serial.print("An interrupt as occurred. Total number: ");
   // Serial.println(totalInterruptCounter);
 
  }
  uint16_t value = tmr1;
  uint16_t value2 = tmr2;
  // calculate signal period in milliseconds
  // 8.0 is Timer1 prescaler and 16000 = MCU_CLK/1000
  period = 16 * value/80000;
  period2 = 16 * value2/80000;
  // calculate signal frequency which is = 1/period ; or = MCU_CLK/(Prescaler * Timer_Value)
  if(value == 0){
    frequency = 0;   // aviod division by zero
  }
  else{
    frequency = 80000000.0/(16UL*value);
  }
  if(value2 == 0){
    frequency2 = 0;   // aviod division by zero
  }
  else{
    frequency2 = 80000000.0/(16UL*value2);
  }
  #endif
 
}
 
// end of code.