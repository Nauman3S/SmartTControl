String serverName;
String channelId;
String port;
String nodeID;
String gatewayID;
String hostName = "cmots";
String apPass="12345678";
String settingsPass;
String apn="";
String apnUser="";
String apnPass="";
String networkType="WiFi";
String network="4G";
String cmotsTemp="0.0";
String cmotsH="0.0";
String cmotsP="0.0";
String cmotsB="0.0";
String cmotsS="0.0";
String sensorTemp="0.0";
String emailAddress="";
String IMEI="";
String enableC="Ena";




String getBroker(){
  return serverName;
}

int getPort(){
  return port.toInt();
}

String getAPN(){
  return apn;
}

#if defined(ARDUINO_ARCH_ESP8266)
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#elif defined(ARDUINO_ARCH_ESP32)
#include <WiFi.h>
#include <WebServer.h>
#endif

#include <AutoConnect.h>

#if defined(ARDUINO_ARCH_ESP8266)
ESP8266WebServer server;
#elif defined(ARDUINO_ARCH_ESP32)
WebServer server;
#endif
#include <ESPmDNS.h>

#include "SoftwareStack.h"
// #ifndef BUILTIN_LED
// #define BUILTIN_LED 2 // backward compatibility
// #endif
#if defined(ARDUINO_ARCH_ESP8266)
#ifdef AUTOCONNECT_USE_SPIFFS
FS &FlashFS = SPIFFS;
#else
#include <LittleFS.h>
FS &FlashFS = LittleFS;
#endif
#elif defined(ARDUINO_ARCH_ESP32)
#include <SPIFFS.h>
fs::SPIFFSFS &FlashFS = SPIFFS;
#endif

#include "statusLED.h"
#include "Controller.h"
#include "neoTimer.h"
#include "PIDController.h"
#include "OLEDHandle.h"
#include "zcdHandler.h"
#include "gprs.h"



#include <ArduinoJson.h>

DynamicJsonDocument doc(1024);
char jsonDoc[1024];

#define GET_CHIPID() ((uint16_t)(ESP.getEfuseMac() >> 32))

unsigned long lastPub = 0;
unsigned int updateInterval = 2000;

#define PARAM_FILE "/param.json"
#define AUX_MQTTSETTING "/mqtt_setting"
#define AUX_MQTTSAVE "/mqtt_save"
#define AUX_MQTTCLEAR "/mqtt_clear"
static const char PAGE_AUTH[] PROGMEM = R"(
{
  "uri": "/auth",
  "title": "Auth",
  "menu": false,
    "auth": "digest",
  "element": [
    {
      "name": "text",
      "type": "ACText",
      "value": "AutoConnect has authorized",
      "style": "font-family:Arial;font-size:18px;font-weight:400;color:#191970"
    }
  ]
}
)";

SoftwareStack ss; //SS instance
AutoConnectConfig config;
AutoConnect portal(server);

String loggedIn = "";

String mac = (WiFi.macAddress());
char __mac[sizeof(mac)];


String devList[10];
String IMEIsList[10];
String LastUpdated = "";
String internetStatus = "Not-Connected";
int selectedDeviceIndex = 0;
String connectionMode = "WiFi";

bool atDetect(IPAddress &softapIP)
{
    Serial.println("Captive portal started, SoftAP IP:" + softapIP.toString());
    LcdPrint("IP",softapIP.toString());

    return true;
}
