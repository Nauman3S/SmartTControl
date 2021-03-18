
volatile int interruptCounter;
int totalInterruptCounter;
 
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
 


uint16_t tmr1 = 0;
float period, frequency;

 const byte interruptPin = 25;
volatile int OpinterruptCounter = 0;
int OpnumberOfInterrupts = 0;
 
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;
 
void IRAM_ATTR OphandleInterrupt() {
  portENTER_CRITICAL_ISR(&mux);
  OpinterruptCounter++;
  tmr1=OpinterruptCounter;
  portEXIT_CRITICAL_ISR(&mux);
  timerRestartN(timer);
}
 

void setupFreq(void) {
 
 
  // // Timer1 module configuration
  // TCCR1A = 0;
  // TCCR1B = 2;   // enable Timer1 module with 1/8 prescaler ( 2 ticks every 1 us)
  // TCNT1  = 0;   // Set Timer1 preload value to 0 (reset)
  // TIMSK1 = 1;   // enable Timer1 overflow interrupt
 
  // EIFR |= 1;  // clear INT0 flag
  // pinMode(18, INPUT_PULLUP);
  // attachInterrupt(18, timer1_get, FALLING);  // enable external interrupt (INT0)


  pinMode(interruptPin, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(interruptPin), OphandleInterrupt, FALLING);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true); //1 000 000 microseconds, is equal to 1 second.
  timerWrite(timer, 0);
  timerAlarmEnable(timer);
}
 

 
// void timer1_get() {
//   tmr1 = TCNT1;
//   TCNT1  = 0;   // reset Timer1
// }
 
// ISR(TIMER1_OVF_vect) {  // Timer1 interrupt service routine (ISR)
//   tmr1 = 0;
// }
 
// main loop
void loopZCD() {
 
  // save current Timer1 value
  if (interruptCounter > 0) {
 
    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);
 
    totalInterruptCounter++;
 
    Serial.print("An interrupt as occurred. Total number: ");
    Serial.println(totalInterruptCounter);
 
  }
  uint16_t value = tmr1;
  // calculate signal period in milliseconds
  // 8.0 is Timer1 prescaler and 16000 = MCU_CLK/1000
  period = 16 * value/80000;
  // calculate signal frequency which is = 1/period ; or = MCU_CLK/(Prescaler * Timer_Value)
  if(value == 0){
    frequency = 0;   // aviod division by zero
  }
  else{
    frequency = 80000000.0/(16UL*value);
  }
  
 
}
 
// end of code.