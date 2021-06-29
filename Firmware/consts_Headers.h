#define USE_WIFI 1
#define USE_GPRS 0

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
#include "Controller.h"
#include "SoftwareStack.h"
#include "settings.h"
#include "PIDController.h"
#include "OLEDHandle.h"
#include "zcdHandler.h"
#ifndef BUILTIN_LED
#define BUILTIN_LED  2  // backward compatibility
#endif

SoftwareStack ss;//SS instance

AutoConnectConfig config;
AutoConnect         portal(server);


String loggedIn="";


String mac=(WiFi.macAddress());
char __mac[sizeof(mac)];    

#if USE_WIFI
WiFiClient wclient;
//PubSubClient mqttClient(wclient);
#endif

String devList[10];
String IMEIsList[10];
String LastUpdated="";
String internetStatus="Not-Connected";
int selectedDeviceIndex=0;
String connectionMode="WiFi";
String APNV="airtelgrps.com";
bool atDetect(IPAddress& softapIP) {
  Serial.println("Captive portal started, SoftAP IP:" + softapIP.toString());
  LcdPrint("IP",softapIP.toString());
  return true;
}

