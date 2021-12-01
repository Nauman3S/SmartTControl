
#define USING_INTERRUPTS 0
volatile int interruptCounter;
int totalInterruptCounter;

#if USING_INTERRUPTS
hw_timer_t *timer = NULL;
portMUX_TYPE timerMux = portMUX_INITIALIZER_UNLOCKED;

void timerRestartN(hw_timer_t *timerT)
{
  // timerT->dev->config.enable = 0;
  // timerT->dev->reload = 1;
  // timerT->dev->config.enable = 1;
  timerAlarmWrite(timer, 1000000, true); //1 000 000 microseconds, is equal to 1 second.
  timerWrite(timer, 0);
  timerAlarmEnable(timer);
}

void IRAM_ATTR onTimer()
{
  portENTER_CRITICAL_ISR(&timerMux);
  interruptCounter++;
  portEXIT_CRITICAL_ISR(&timerMux);
}
#endif

uint16_t tmr1 = 0;
float period, frequency;

const byte interruptPin = 25;
volatile int OpinterruptCounter = 0;
int OpnumberOfInterrupts = 0;

#if USING_INTERRUPTS
portMUX_TYPE mux = portMUX_INITIALIZER_UNLOCKED;

void IRAM_ATTR OphandleInterrupt()
{
  portENTER_CRITICAL_ISR(&mux);
  OpinterruptCounter++;
  tmr1 = OpinterruptCounter;
  portEXIT_CRITICAL_ISR(&mux);
  timerRestartN(timer);
}

#endif
void setupFreq(void)
{

  pinMode(interruptPin, INPUT_PULLUP);
#if USING_INTERRUPTS
  attachInterrupt(digitalPinToInterrupt(interruptPin), OphandleInterrupt, FALLING);

  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 1000000, true); //1 000 000 microseconds, is equal to 1 second.
  timerWrite(timer, 0);
  timerAlarmEnable(timer);
#endif
}

float getFrequency()
{
  return frequency;
}
void loopZCD()
{
#if USING_INTERRUPTS
  // save current Timer1 value
  if (interruptCounter > 0)
  {

    portENTER_CRITICAL(&timerMux);
    interruptCounter--;
    portEXIT_CRITICAL(&timerMux);

    totalInterruptCounter++;

    //   Serial.print("An interrupt as occurred. Total number: ");
    // Serial.println(totalInterruptCounter);
  }
  uint16_t value = tmr1;
  // calculate signal period in milliseconds
  // 8.0 is Timer1 prescaler and 16000 = MCU_CLK/1000
  period = 16 * value / 80000;
  // calculate signal frequency which is = 1/period ; or = MCU_CLK/(Prescaler * Timer_Value)
  if (value == 0)
  {
    frequency = 0; // aviod division by zero
  }
  else
  {
    frequency = 80000000.0 / (16UL * value);
  }
#endif
}

// end of code.