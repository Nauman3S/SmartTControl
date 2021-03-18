#include <EEPROM.h>
#define EEPROM_SIZE 512

void setupEEPROM(){
    EEPROM.begin(EEPROM_SIZE);
}


void writeToEEPROM(int index, int val){
    EEPROM.write(index, val);
    EEPROM.commit();
}

int readFromEEPROM(int index){
    int a=EEPROM.read(index);
    return a;
}