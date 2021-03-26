
#include "consts_Headers.h"
#include "EEPROMHandler.h"
#include "MQTTFuncs.h"
#include "webApp.h"


void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  setupFreq();
  SetupRelay();
  setupDS18B20();
  setupOLED();
  setupPID();
  setupEEPROM();
  
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
  server.on("/dev", handleDEV);
  server.on("/conn", handleCON);
  server.on("/cmots", cmotsValues);

  // Starts user web site included the AutoConnect portal.
  portal.onDetect(atDetect);
  if (portal.begin()) {
    Serial.println("Started, IP:" + WiFi.localIP().toString());
    LcdPrint("IP:",WiFi.localIP().toString());
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
  if(devList[0]!=String("0")){//check if data received from the cmots platform
  loopPID();
  }
  loopZCD();
  
  if (!mqttClient.connected()) {
    reconnect();
  }
  mqttClient.loop();
  if (WiFi.status() == WL_IDLE_STATUS) {
    ESP.restart();

    delay(1000);
  }
}
