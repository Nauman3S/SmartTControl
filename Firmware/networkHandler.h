#define TINY_GSM_MODEM_SIM800

// See all AT commands, if wanted
//#define DUMP_AT_COMMANDS
#define SUPRESS_LOGS 1

// Define the serial console for debug prints, if needed
#define TINY_GSM_DEBUG SerialMon

// Range to attempt to autobaud
#define GSM_AUTOBAUD_MIN 115200
#define GSM_AUTOBAUD_MAX 115200

// Add a reception delay, if needed
#define TINY_GSM_YIELD() { vTaskDelay(2); }

// Set serial for debug console (to the Serial Monitor, default speed 115200)
#define SerialMon Serial

// Set serial for AT commands (to the module)
// Use Hardware Serial on Mega, Leonardo, Micro
#define SerialAT Serial2

// Your GPRS credentials
// Leave empty, if missing user or pass
const char apn[]  = "zonginternet";//"jazzconnect.mobilinkworld.com";//"zonginternet";
const char user[] = "";
const char pass[] = "";

// MQTT details
#if USE_GPRS
#include <TinyGsmClient.h>
//#include <PubSubClient.h>already included
#endif

#ifdef DUMP_AT_COMMANDS
  #include <StreamDebugger.h>
  StreamDebugger debugger(SerialAT, SerialMon);
  TinyGsm modem(debugger);
#else


#endif
#if USE_GPRS
TinyGsm modem(SerialAT);
TinyGsmClient client(modem);
PubSubClient mqtt(client);
#endif
void setupGPRS() {
#if USE_GPRS
SerialAT.begin(115200);
  vTaskDelay(1000);
#endif
  // Restart takes quite some time
  // To skip it, call init() instead of restart()
  //SerialMon.println("Initializing modem...");
  //modem.restart();
   
  // MQTT Broker setup
}
void initModem(){
    #if USE_GPRS
    modem.init();
    if (!modem.gprsConnect(apn, user, pass)) {
    //SerialMon.println(" fail");
    
      vTaskDelay(5000);
}
#endif
}

void powerOffModem(){
    // Try to power-off (modem may decide to restart automatically)
  // To turn off modem completely, please use Reset/Enable pins
  #if USE_GPRS
     modem.poweroff();
     #endif
}