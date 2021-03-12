
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
void MQTTUnSubscribe(){
    String topicN=String("SmartTControl/data/devices/")+OLDemailAddress;
    String topicU=String("SmartTControl/lastUpdated/devices/")+OLDemailAddress;
    for(int i=0;i<10;i++){
       IMEIsList[i]=String("NA");
       devList[i]=String("0");
    }
    mqttClient.unsubscribe(topicN.c_str());
    mqttClient.unsubscribe(topicU.c_str());
    
}
void MQTTSubscriptions(){
  //mqttClient.subscribe("SmartTControl/data/v");
  
  // for(int i=0;i<10;i++){
  //   IMEIsList[i]==String("NA");
  // }
  String topicN=String("SmartTControl/data/devices/")+emailAddress;
  String topicU=String("SmartTControl/lastUpdated/devices/")+emailAddress;
  mqttClient.subscribe(topicN.c_str());
  mqttClient.subscribe(topicU.c_str());
  
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

  else if(String(topic)==String("SmartTControl/data/devices/")+emailAddress){
    
    
      nD=getNumberOfDevices(pLoad);
      Serial.print("Number of Devices::");
      Serial.println(nD);
      if(nD==1){
      Serial.print("Data :::");
      Serial.println(String(pLoad));
      devList[0]=ss.StringSeparator(pLoad,';',1);//1 is temp 0 is imei
      IMEIsList[0]=ss.StringSeparator(pLoad,';',0);//1 is temp 0 is imei
      Serial.println(devList[0]);
      }
      if(nD>1){
        for (int i=0;i<nD;i++){
            String IMEIs=ss.StringSeparator(pLoad,';',0);
            String Temps=ss.StringSeparator(pLoad,';',1);

            devList[i]=ss.StringSeparator(Temps,',',i);

            Serial.println(devList[i]);
        

            IMEIsList[i]=ss.StringSeparator(IMEIs,',',i);
            Serial.println(IMEIsList[i]);
        //IMEIsList[1]=ss.StringSeparator(IMEIs,',',1);

        }
        
      }


      
      // dev1=ss.StringSeparator(pLoad,';',0);
      // dev2=ss.StringSeparator(pLoad,';',1);
      // Serial.println(dev1);
      // Serial.println(dev2);
      changeRelayState(compareValues(devList[0].toFloat(),getTempValue()));

    // }
  }

      else if(String(topic)==String("SmartTControl/lastUpdated/devices/")+emailAddress){
          LastUpdated=pLoad;
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
    page += String(F("<h2>Account: "));
    page += String((emailAddress));
    page += String(F("</h2><br><br>"));

    page += String(F("<h6>Data From Server Received At: "));
    page += String((LastUpdated));
    page += String(F("</h6><br><br>"));

    for (int j=0;j<nD;j++){

    page += String(F("<h4>Sensor "));
    page += String((j));
    page += String("</h4><br>  ");
    page += String(F("<h5>IMEI: "));
    page += String((IMEIsList[j]));
    page += String(F("</h5><br>"));
    page += String(F("<h5>Temperature: "));
    page += String((devList[j]));
    page += String(F("</h5><br><br>"));
    
    }

    // page += String(F("<h4>Sensor 2</h4><br>  "));
    // page += String(F("<h5>IMEI: "));
    // page += String((IMEIsList[1]));
    // page += String(F("</h5><br>"));
    // page += String(F("<h5>Temperature: "));
    // page += String((dev2));
    // page += String(F("</h5><br><br>"));
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
  OLDemailAddress=emailAddress;
  String em=String(server.arg("email"));
  emailAddress=em;
  String pas=String(server.arg("pass"));
  String freq=String(server.arg("freq"));
  Serial.println(em);
  Serial.println(pas);
  String dataV=String(em);
  dataV+=String(";");
  dataV+=String(pas);
  mqttPublish("SmartTControl/creds/data",dataV);
  //mqttPublish("SmartTControl/device/freq",freq);
  //mqttPublish("SmartTControl/device/run","3");//to run
  //reconnect();//for new email address subscription
  MQTTUnSubscribe();
  delay(0.5);
  MQTTSubscriptions();
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
