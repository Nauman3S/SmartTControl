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
#include <PubSubClient.h>
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

const char *mqtt_server = "broker.hivemq.com";
const int mqtt_port = 1883;
const char *mqtt_user = "testUser";
const char *mqtt_pass = "testUser@123";
const char *mqtt_client_name = __mac;//"12312312312332212";// any random alphanumeric stirng
//////////////////////////////
#define BUFFER_SIZE 250
String incoming="";
String incomingTopic="";
WiFiClient wclient;
PubSubClient mqttClient(wclient);

String devList[10];
String IMEIsList[10];
String LastUpdated="";
String internetStatus="Not-Connected";
int selectedDeviceIndex=0;

bool atDetect(IPAddress& softapIP) {
  Serial.println("Captive portal started, SoftAP IP:" + softapIP.toString());
  LcdPrint("IP",softapIP.toString());
  return true;
}

