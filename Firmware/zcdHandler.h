#include <LiquidCrystal.h>    // include Arduino LCD library
 
// LCD module connections (RS, E, D4, D5, D6, D7)

 
void setupFreq(void) {
 
 
  // Timer1 module configuration
  TCCR1A = 0;
  TCCR1B = 2;   // enable Timer1 module with 1/8 prescaler ( 2 ticks every 1 us)
  TCNT1  = 0;   // Set Timer1 preload value to 0 (reset)
  TIMSK1 = 1;   // enable Timer1 overflow interrupt
 
  EIFR |= 1;  // clear INT0 flag
  pinMode(18, INPUT_PULLUP);
  attachInterrupt(18, timer1_get, FALLING);  // enable external interrupt (INT0)
}
 
uint16_t tmr1 = 0;
float period, frequency;
 
void timer1_get() {
  tmr1 = TCNT1;
  TCNT1  = 0;   // reset Timer1
}
 
ISR(TIMER1_OVF_vect) {  // Timer1 interrupt service routine (ISR)
  tmr1 = 0;
}
 
// main loop
void loop() {
 
  // save current Timer1 value
  uint16_t value = tmr1;
  // calculate signal period in milliseconds
  // 8.0 is Timer1 prescaler and 16000 = MCU_CLK/1000
  period = 8.0 * value/16000;
  // calculate signal frequency which is = 1/period ; or = MCU_CLK/(Prescaler * Timer_Value)
  if(value == 0)
    frequency = 0;   // aviod division by zero
  else
    frequency = 16000000.0/(8UL*value);
 
  lcd.setCursor(7, 0);
  lcd.print(frequency);
  lcd.print(" Hz  ");
  // print period
  lcd.setCursor(7, 1);
  lcd.print(period);
  lcd.print(" ms  ");
  
  delay(500);
 
}
 
// end of code.