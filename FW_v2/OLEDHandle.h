  
///////////////////////////////////////////
// Libraries 
///////////////////////////////////////////
#include <Wire.h>
#include "SSD1306Wire.h"


///////////////////////////////////////////
// OLED display
///////////////////////////////////////////

SSD1306Wire  display(0x3c, 14, 27);


///////////////////////////////////
// Prototype Functions
///////////////////////////////////

void INIT_DISPLAY();
void READYDISPALY();
void PRINT_ANY_DISPALY(String msg);
void LcdPrint(String display1,String display2);
void drawRect(void);
void fillRect(void);
void setupOLED(void);


///////////////////////////////////
// INIT_DISPLAY
///////////////////////////////////

void INIT_DISPLAY()
{
  display.init();
  display.setFont(ArialMT_Plain_10);
  display.setContrast(255);
}

void READYDISPALY()
{

  drawRect();
  vTaskDelay(1000);
  display.clear();

  fillRect();
  vTaskDelay(1000);
  display.clear();

 /* display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(5, 10,"Ready Payment!");
  display.display();
  */
  LcdPrint("Connecting","Wait...");
  
}
void setupOLED(void){
  INIT_DISPLAY();
  READYDISPALY();
}
void PRINT_ANY_DISPALY(String msg)
{

  display.clear();
  display.setTextAlignment(TEXT_ALIGN_LEFT);
  display.setFont(ArialMT_Plain_16);
  display.drawString(5, 10,msg);
  display.display();
  
}

void LcdPrint(String display1,String display2){
  
 display.clear();
 display.setTextAlignment(TEXT_ALIGN_LEFT);
 display.setFont(ArialMT_Plain_16);
 display.drawString(0, 10,display1);
 display.drawString(0, 30,display2);
 display.display();

 }

 void drawRect(void) {
  for (int16_t i=0; i<display.getHeight()/2; i+=2) {
    display.drawRect(i, i, display.getWidth()-2*i, display.getHeight()-2*i);
    display.display();
    vTaskDelay(10);
  }
}

void fillRect(void) {
  uint8_t color = 1;
  for (int16_t i=0; i<display.getHeight()/2; i+=3) {
    display.setColor((color % 2 == 0) ? BLACK : WHITE); // alternate colors
    display.fillRect(i, i, display.getWidth() - i*2, display.getHeight() - i*2);
    display.display();
    vTaskDelay(10);
    color++;
  }
  // Reset back to WHITE
  display.setColor(WHITE);
}
