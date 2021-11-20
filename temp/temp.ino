
#include "oledHandler.h"
void setup()
{
    Serial.begin(115200);
    setupOLED();
    LcdPrint("P1", "P2");
}

void loop()
{
    //LcdPrint("P1", "P2");
    int a=0;
    delay(100);
}