
#include "consts_Headers.h"
#include "EEPROMHandler.h"
#include "wifiPostReq.h"
#include "webApp.h"
#include "gprsPostReq.h"

void setup()
{
  delay(3000);
  Serial.begin(115200);
  Serial.println();
  pinMode(BUILTIN_LED, OUTPUT);
  for (int i = 0; i < 5; i++)
  {
    pinMode(BUILTIN_LED, HIGH);
    delay(500);
    pinMode(BUILTIN_LED, LOW);
    delay(500);
  }
  setupWiFiHTTP();
  setupFreq();
  SetupRelay();
  setupDS18B20();
  setupOLED();
  setupGPRS();

  setupPID();
  setupEEPROM();

  if (!MDNS.begin("esp32"))
  {
    Serial.println("Error setting up MDNS responder!");
    while (1)
    {
      delay(1000);
    }
  }
  portal.config("ESP32AP", "123456789AP");
  config.title = "SmartTControl";
  portal.append("/cmots", "Cmots");

  server.on("/", handleRoot);
  server.on("/io", handleGPIO);
  server.on("/dev", handleDEV);
  server.on("/conn", handleCON);
  server.on("/cmots", cmotsValues);

  // Starts user web site included the AutoConnect portal.
  portal.onDetect(atDetect);
  if (portal.begin())
  {
    Serial.println("Started, IP:" + WiFi.localIP().toString());
    LcdPrint("IP:", WiFi.localIP().toString());
  }
  else
  {
    Serial.println("Connection failed.");
    while (true)
    {
      yield();
    }
  }

  MDNS.addService("http", "tcp", 80);
  //mqttConnect();
}

void loop()
{
  server.handleClient();
  portal.handleRequest(); // Need to handle AutoConnect menu.
  //mqttPublish("SmartTControl/d/v","success");
  if (connectionMode == String("WiFi"))
  {
    if (WiFi.status() != WL_CONNECTED)
    {
      internetStatus = String("Not Connected!");
    }
    else
    {
      internetStatus = String("Connected!");
      postReq();
    }

    if (WiFi.status() == WL_IDLE_STATUS)
    {
      ESP.restart();

      delay(1000);
    }
  }

  else if (connectionMode == String("GPRS"))
  {
    loopGPRS();
  }

  if (devList[0] != String("0"))
  { //check if data received from the cmots platform
    loopPID();
  }
  loopZCD();

  // if (!mqttClient.connected()) {
  //   reconnect();
  // }
  // mqttClient.loop();
}
