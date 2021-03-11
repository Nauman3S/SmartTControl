
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

String dev1="";
String dev2="";
void MQTTSubscriptions(){
  mqttClient.subscribe("SmartTControl/data/v");
  mqttClient.subscribe("SmartTControl/user/login");
  
}
void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String pLoad="";
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    pLoad=pLoad+String((char)payload[i]);
  }
  Serial.println();
  if(String(topic)==String("SmartTControl/user/login")){
    Serial.print("login status:::");
    Serial.println(String(pLoad));
    loggedIn=pLoad;
  }

  else if(String(topic)==String("SmartTControl/data/v")){
    
    if(loggedIn==String("yes")){
      //if user is logged into cmost.ca
      Serial.print("Data :::");
      Serial.println(String(pLoad));
      dev1=ss.StringSeparator(pLoad,';',0);
      dev2=ss.StringSeparator(pLoad,';',1);
      Serial.println(dev1);
      Serial.println(dev2);
      changeRelayState(compareValues(dev1.toFloat(),getTempValue()));

    }
  }
  // Switch on the LED if an 1 was received as first character
  if ((char)payload[0] == '1') {
    digitalWrite(BUILTIN_LED, LOW);   // Turn the LED on (Note that LOW is the voltage level
    // but actually the LED is on; this is because
    // it is active low on the ESP-01)
  } else {
    digitalWrite(BUILTIN_LED, HIGH);  // Turn the LED off by making the voltage HIGH
  }
  pLoad="";

}
void reconnect() {
  // Loop until we're reconnected
  while (!mqttClient.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "ESP8266Client-";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
   if (mqttClient.connect(clientId.c_str(), mqtt_user, mqtt_pass)) {
      Serial.println("Established:" + String(clientId));
      //mqttClient.subscribe("SmartTControl/data/v");
      MQTTSubscriptions();
     // return true;
    
    } else {
      Serial.print("failed, rc=");
      Serial.print(mqttClient.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}
bool mqttConnect() {
  static const char alphanum[] = "0123456789"
    "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
    "abcdefghijklmnopqrstuvwxyz";  // For random generation of client ID.
  char    clientId[9];

  uint8_t retry = 3;
  while (!mqttClient.connected()) {
    if (String(mqtt_server).length() <= 0)
      break;

    mqttClient.setServer(mqtt_server, 1883);
    mqttClient.setCallback(callback);
    Serial.println(String("Attempting MQTT broker:") + String(mqtt_server));

    for (uint8_t i = 0; i < 8; i++) {
      clientId[i] = alphanum[random(62)];
    }
    clientId[8] = '\0';

    if (mqttClient.connect(clientId, mqtt_user, mqtt_pass)) {
      Serial.println("Established:" + String(clientId));
      //mqttClient.subscribe("SmartTControl/data/v");
      MQTTSubscriptions();
      return true;
    }
    else {
      Serial.println("Connection failed:" + String(mqttClient.state()));
      if (!--retry)
        break;
      delay(3000);
    }
  }
  return false;
}

void mqttPublish(String path,String msg) {
  //String path = String("channels/") + channelId + String("/publish/") + apiKey;
  mqttClient.publish(path.c_str(), msg.c_str());
}
void cmotsValues(){

   String page = PSTR(
"<html>"
"</head>"
  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
  "<style type=\"text/css\">"
    "body {"
    "-webkit-appearance:none;"
    "-moz-appearance:none;"
    "font-family:'Arial',sans-serif;"
    "text-align:center;"
    "}"
    ".menu > a:link {"
    "position: absolute;"
    "display: inline-block;"
    "right: 12px;"
    "padding: 0 6px;"
    "text-decoration: none;"
    "}"
    ".button {"
    "display:inline-block;"
    "border-radius:7px;"
    "background:#73ad21;"
    "margin:0 10px 0 10px;"
    "padding:10px 20px 10px 20px;"
    "text-decoration:none;"
    "color:#000000;"
    "}"
  "</style>"
"</head>"
"<body>"
  "<div class=\"menu\">" AUTOCONNECT_LINK(BAR_24) "</div>"
  "Smart T Controller<br>");

    page += String(F("</body></html>"));

    page += String(F("<h3>Live data from Cmots.ca</h3><br><br>"));

    page += String(F("<h4>Sensor 1: "));
    page += String((dev1));
    page += String(F("</h4><br><br>"));

    page += String(F("<h4>Sensor 2: "));
    page += String((dev2));
    page += String(F("</h4><br><br>"));
    page += String(F("<p><br><a class=\"button\" href=\"/cmots\">Refresh</a></p>"));

  
    server.send(200, "text/html", page);


}
void handleRoot() {
  String page = PSTR(
"<html>"
"</head>"
  "<meta name=\"viewport\" content=\"width=device-width, initial-scale=1\">"
  "<style type=\"text/css\">"
    "body {"
    "-webkit-appearance:none;"
    "-moz-appearance:none;"
    "font-family:'Arial',sans-serif;"
    "text-align:center;"
    "}"
    ".menu > a:link {"
    "position: absolute;"
    "display: inline-block;"
    "right: 12px;"
    "padding: 0 6px;"
    "text-decoration: none;"
    "}"
    ".button {"
    "display:inline-block;"
    "border-radius:7px;"
    "background:#73ad21;"
    "margin:0 10px 0 10px;"
    "padding:10px 20px 10px 20px;"
    "text-decoration:none;"
    "color:#000000;"
    "}"
  "</style>"
"</head>"
"<body>"
  "<div class=\"menu\">" AUTOCONNECT_LINK(BAR_24) "</div>"
  "Smart T Controller<br>"
  "GPIO(");
  page += String(BUILTIN_LED);
  page += String(F(") : <span style=\"font-weight:bold;color:"));
  page += digitalRead(BUILTIN_LED) ? String("Tomato\">HIGH") : String("SlateBlue\">LOW");
  page += String(F("</span>"));
  page += String(F("<p>Server State<br><a class=\"button\" href=\"/io?v=low\">STOP</a><a class=\"button\" href=\"/io?v=high\">RUN</a></p>"));


  page += String(F("<h3>Enter your cmots.ca credentials and data freq. below</h3><br><br><form action=/io?v><input type=\"text\" id=\"email\" name=\"email\" placeholder=\"Email Address\"><br><br>    <input placeholder=\"Password\" type=\"text\" id=\"pass\" name=\"pass\"><br><br> <input type=\"text\" id=\"freq\" name=\"freq\" placeholder=\"Data Freq(sec).\" value=\"1.5\"><br><br>  <input type=\"submit\" value=\"Submit\"></form><br><br><p>Press RUN after 5 seconds of submitting creds.</p>"));

  page += String(F("</body></html>"));

  
  server.send(200, "text/html", page);
}

void sendRedirect(String uri) {
  server.sendHeader("Location", uri, true);
  server.send(302, "text/plain", "");
  server.client().stop();
}

void handleGPIO() {
  String em=String(server.arg("email"));
  String pas=String(server.arg("pass"));
  String freq=String(server.arg("freq"));
  Serial.println(em);
  Serial.println(pas);
  String dataV=String(em);
  dataV+=String(";");
  dataV+=String(pas);
  mqttPublish("SmartTControl/creds/data",dataV);
  mqttPublish("SmartTControl/device/freq",freq);
  mqttPublish("SmartTControl/device/run","3");//to run
  if (server.arg("v") == "low"){
    digitalWrite(BUILTIN_LED, LOW);
    mqttPublish("SmartTControl/device/run","0");//to off
  }
  else if (server.arg("v") == "high"){
    digitalWrite(BUILTIN_LED, HIGH);
    mqttPublish("SmartTControl/device/run","1");//to off
    }
   else{
    
   }
   
  sendRedirect("/");
}

bool atDetect(IPAddress& softapIP) {
  Serial.println("Captive portal started, SoftAP IP:" + softapIP.toString());
  return true;
}


void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  SetupRelay();
  setupDS18B20();
  
  pinMode(BUILTIN_LED, OUTPUT);

  // Put the home location of the web site.
  // But in usually, setting the home uri is not needed cause default location is "/".
  //portal.home("/");   
  //  if (portal.begin()) {
  //   if (MDNS.begin("esp32")) {
  //     MDNS.addService("http", "tcp", 80);
  //   }
  //   else{
  //     delay(1000);
  //   }
  // }
  if (!MDNS.begin("esp32")) {
        Serial.println("Error setting up MDNS responder!");
        while(1) {
            delay(1000);
        }
    }
  portal.config("ESP32AP","123456789AP");
  config.title = "SmartTControl";
  portal.append("/cmots", "Cmots");
 

  server.on("/", handleRoot);
  server.on("/io", handleGPIO);
  server.on("/cmots", cmotsValues);

  // Starts user web site included the AutoConnect portal.
  portal.onDetect(atDetect);
  if (portal.begin()) {
    Serial.println("Started, IP:" + WiFi.localIP().toString());
  }
  else {
    Serial.println("Connection failed.");
    while (true) { yield(); }
  }

   MDNS.addService("http", "tcp", 80);
    mqttConnect();

   
}

void loop() {
  server.handleClient();
  portal.handleRequest();   // Need to handle AutoConnect menu.
  //mqttPublish("SmartTControl/d/v","success");
  
  
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  if (WiFi.status() == WL_IDLE_STATUS) {
#if defined(ARDUINO_ARCH_ESP8266)
    ESP.reset();
#elif defined(ARDUINO_ARCH_ESP32)
    ESP.restart();
#endif
    delay(1000);
  }
}
