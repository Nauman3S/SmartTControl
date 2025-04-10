#include "headers.h" //all misc. headers and functions
#include "wifiPostReq.h"
#include "webApp.h" //Captive Portal webpages
#include <FS.h>     //ESP32 File System
//CT SB360 to vin pin
IPAddress ipV(192, 168, 4, 1);

TaskHandle_t Task1;
Neotimer n1(2000);
Neotimer n2(3000);
Neotimer n3(5000);
Neotimer n4(7000);
String loadParams(AutoConnectAux &aux, PageArgument &args) //function to load saved settings
{
    (void)(args);
    File param = FlashFS.open(PARAM_FILE, "r");

    if (param)
    {
        Serial.println("load params func");
        aux.loadElement(param);
        Serial.println(param);
        AutoConnectText &networkElm = aux["network"].as<AutoConnectText>();
        AutoConnectText &providerElm = aux["provider"].as<AutoConnectText>();
        AutoConnectText &signalElm = aux["signal"].as<AutoConnectText>();
        AutoConnectText &sensorT = aux["sensorT"].as<AutoConnectText>();
        AutoConnectText &cmotsT = aux["cmotsT"].as<AutoConnectText>();
        AutoConnectText &cmotsHM = aux["cmotsH"].as<AutoConnectText>();
        AutoConnectText &cmotsPM = aux["cmotsP"].as<AutoConnectText>();
        AutoConnectText &cmotsBM = aux["cmotsB"].as<AutoConnectText>();
        AutoConnectText &cmotsSM = aux["cmotsS"].as<AutoConnectText>();

        networkElm.value = String("Network: ") + network;
        // providerElm.value = String("Provider: ") + provider;
        // signalElm.value = String("Signal: ") + signal;
        sensorT.value = String("Sensor Temperature: ") + sensorTemp;
        cmotsT.value = String("cmots Temperature: ") + cmotsTemp;
        cmotsHM.value = String("cmots Humidity: ") + cmotsH;
        cmotsPM.value = String("cmots Pressure: ") + cmotsP;
        cmotsBM.value = String("cmots Battery: ") + cmotsB;
        cmotsSM.value = String("cmots ServerTime: ") + cmotsS;

        // curSValueElm.value="CurS:7788";
        param.close();
    }
    else
        Serial.println(PARAM_FILE " open failed");
    return String("");
}

String saveParams(AutoConnectAux &aux, PageArgument &args) //save the settings
{
    serverName = args.arg("mqttserver"); //broker
    serverName.trim();

    gatewayID = args.arg("gatewayID");
    gatewayID.trim();

    port = args.arg("port"); //user name
    port.trim();

    nodeID = args.arg("nodeID"); //password
    nodeID.trim();

    apPass = args.arg("apPass"); //ap pass
    apPass.trim();

    settingsPass = args.arg("settingsPass"); //ap pass
    settingsPass.trim();

    hostName = args.arg("hostname");
    hostName.trim();

    apn = args.arg("apn");
    apn.trim();
    apnUser = args.arg("apnUser");
    apnUser.trim();
    apnPass = args.arg("apnPass");
    apnPass.trim();

    emailAddress = args.arg("emailAddress");
    emailAddress.trim();
    IMEI = args.arg("IMEI");
    IMEI.trim();

    networkType = args.arg("networkType");
    networkType.trim();

    // The entered value is owned by AutoConnectAux of /mqtt_setting.
    // To retrieve the elements of /mqtt_setting, it is necessary to get
    // the AutoConnectAux object of /mqtt_setting.
    File param = FlashFS.open(PARAM_FILE, "w");
    portal.aux("/mqtt_setting")->saveElement(param, {"mqttserver", "gatewayID", "port", "nodeID", "hostname", "apPass", "settingsPass", "apn", "apnUser", "apnPass", "networkType", "emailAddress", "IMEI"});
    param.close();

    // Echo back saved parameters to AutoConnectAux page.
    AutoConnectText &echo = aux["parameters"].as<AutoConnectText>();
    echo.value = "Server: " + serverName + "<br>";
    echo.value += "Gateway ID: " + gatewayID + "<br>";
    echo.value += "Port: " + port + "<br>";
    echo.value += "Node ID: " + nodeID + "<br>";
    echo.value += "ESP host name: " + hostName + "<br>";
    echo.value += "AP Password: " + apPass + "<br>";
    echo.value += "Settings Page Password: " + settingsPass + "<br>";

    echo.value += "APN: " + apn + "<br>";
    echo.value += "APN Username: " + apnUser + "<br>";
    echo.value += "APN Password: " + apnPass + "<br>";
    echo.value += "Network Type: " + networkType + "<br>";
    echo.value += "CMOTS EMAIL: " + emailAddress + "<br>";
    echo.value += "CMOTS IMEI: " + IMEI + "<br>";

    return String("");
}
bool loadAux(const String auxName) //load defaults from data/*.json
{
    bool rc = false;
    Serial.println("load aux func");
    String fn = auxName + ".json";
    File fs = FlashFS.open(fn.c_str(), "r");
    if (fs)
    {
        rc = portal.load(fs);
        fs.close();
    }
    else
        Serial.println("Filesystem open failed: " + fn);
    return rc;
}

void Task1Loop(void *pvParameters) //GPRS
{
    Serial.print("Task1 running on core ");
    Serial.println(xPortGetCoreID());
    setupGPRS();
    yield();

    for (;;)
    {
        //loopGPRS();
        //capacity = String(getADC(5));
        if (isMQTTConnected())
        {
            yield();
            delay(1000);
        }
    }
}

uint8_t inAP = 0;
unsigned long tmDetection;
const unsigned long scanInterval = 10 * 1000;
uint8_t wifiReconnection()
{
    int16_t ns = WiFi.scanComplete();

    if (ns == WIFI_SCAN_RUNNING)
    {
        // The scan is still running, nothing to do.
    }
    else if (ns == WIFI_SCAN_FAILED)
    {
        // The scan will start in the background where it has not started.
        // But it will wait for scanInterval from the last scan
        if (millis() - tmDetection > scanInterval)
        {
            WiFi.disconnect();
            WiFi.scanNetworks(true, true, false);
        }
    }
    else
    {
        // Now that the scan is finished, you can start checking if any known APs
        // have been detected.
        Serial.printf("scanNetworks:%d\n", ns);

        int16_t scanResult = 0;
        while (scanResult < ns)
        {
            AutoConnectCredential cred;
            station_config_t staConfig;

            if (cred.load(WiFi.SSID(scanResult++).c_str(), &staConfig) >= 0)
            {
                // Successfully detected a known AP, aborting captive portal,
                // Relying on autoReconnect and reconnectInterval to reconnect.
                Serial.printf("AP %s ready\n", (char *)staConfig.ssid);
                WiFi.scanDelete();
                return false;
            }
        }

        Serial.println("No found known AP");
        WiFi.scanDelete();
        tmDetection = millis();
    }

    return true;
}
bool whileCP()
{

    //use this function as a main loop

    ledState(AP_MODE);
    if (n1.repeat())
    {
        //LcdPrint("IP: ", "192.168.4.1");//fixed IP in captive portal

        if (inAP == 0)
        {
            LcdPrint("Status: ", "AP Mode");
            Serial.println("Status: AP Mode");
            inAP = 1;
        }
        else if (inAP == 1)
        {
            LcdPrint("IP: ", "192.168.4.1"); //fixed IP in captive portal
            Serial.println("IP : 192.168.4.1");
            inAP = 0;
        }
    }

    // inAP = 1;

    loopLEDHandler();
    return wifiReconnection();
}

void setup() //main setup functions
{
    Serial.begin(115200);

    xTaskCreatePinnedToCore(
        Task1Loop, /* Task function. */
        "Task1",   /* name of task. */
        10000,     /* Stack size of task */
        NULL,      /* parameter of the task */
        1,         /* priority of the task */
        &Task1,    /* Task handle to keep track of created task */
        1);        /* pin task to core 1 */
    delay(500);

    SetupRelay();
    setupDS18B20();
    setupOLED();
    setupWiFiHTTP();
    setupFreq();
    setupHyst();
    delay(1000);

    if (!MDNS.begin("cmots")) //starting mdns so that user can access webpage using url `esp32.local`(will not work on all devices)
    {
        Serial.println("Error setting up MDNS responder!");
        while (1)
        {
            delay(1000);
        }
    }
#if defined(ARDUINO_ARCH_ESP8266)
    FlashFS.begin();
#elif defined(ARDUINO_ARCH_ESP32)
    FlashFS.begin(true);
#endif
    loadAux(AUX_MQTTSETTING);
    loadAux(AUX_MQTTSAVE);
    AutoConnectAux *setting = portal.aux(AUX_MQTTSETTING);
    if (setting) //get all the settings parameters from setting page on esp32 boot
    {
        Serial.println("Setting loaded");
        PageArgument args;
        AutoConnectAux &mqtt_setting = *setting;
        loadParams(mqtt_setting, args);
        AutoConnectInput &hostnameElm = mqtt_setting["hostname"].as<AutoConnectInput>();
        AutoConnectInput &apPassElm = mqtt_setting["apPass"].as<AutoConnectInput>();
        AutoConnectInput &serverNameElm = mqtt_setting["mqttserver"].as<AutoConnectInput>();
        AutoConnectInput &portElm = mqtt_setting["port"].as<AutoConnectInput>();
        AutoConnectInput &gwIDElm = mqtt_setting["gatewayID"].as<AutoConnectInput>();
        AutoConnectInput &noIDElm = mqtt_setting["nodeID"].as<AutoConnectInput>();
        AutoConnectInput &settingsPassElm = mqtt_setting["settingsPass"].as<AutoConnectInput>();

        AutoConnectInput &apnElm = mqtt_setting["apn"].as<AutoConnectInput>();

        AutoConnectInput &apnUserElm = mqtt_setting["apnUser"].as<AutoConnectInput>();
        AutoConnectInput &apnPassElm = mqtt_setting["apnPass"].as<AutoConnectInput>();
        AutoConnectInput &emailAddressElm = mqtt_setting["emailAddress"].as<AutoConnectInput>();
        AutoConnectInput &IMEIElm = mqtt_setting["IMEI"].as<AutoConnectInput>();

        AutoConnectRadio &networkTypeElm = mqtt_setting["networkType"].as<AutoConnectRadio>();

        AutoConnectText &networkElm = mqtt_setting["network"].as<AutoConnectText>();
        // AutoConnectText &providerElm = mqtt_setting["provider"].as<AutoConnectText>();
        // AutoConnectText &signalElm = mqtt_setting["signal"].as<AutoConnectText>();

        AutoConnectText &sensorTElm = mqtt_setting["sensorT"].as<AutoConnectText>();
        AutoConnectText &cmotsTElm = mqtt_setting["cmotsT"].as<AutoConnectText>();
        //vibSValueElm.value="VibS:11";
        serverName = String(serverNameElm.value);
        port = String(portElm.value);
        gatewayID = String(gwIDElm.value);
        nodeID = String(noIDElm.value);
        hostName = String(hostnameElm.value);
        apPass = String(apPassElm.value);
        settingsPass = String(settingsPassElm.value);
        apnUser = String(apnUserElm.value);
        apnPass = String(apnPassElm.value);
        apn = String(apnElm.value);

        emailAddress = String(emailAddressElm.value);
        IMEI = String(IMEIElm.value);

        networkType = String(networkTypeElm.value());

        if (hostnameElm.value.length())
        {
            //hostName=hostName+ String("-") + String(GET_CHIPID(), HEX);
            //;
            //portal.config(hostName.c_str(), apPass.c_str());
            // portal.config(hostName.c_str(), "123456789AP");
            config.apid = hostName + "-" + String(GET_CHIPID(), HEX);
            config.password = apPass;
            config.psk = apPass;
            // portal.config(hostName.c_str(), "123456789AP");
            Serial.println("(from hostELM) hostname set to " + hostName);
        }
        else
        {

            // hostName = String("OEE");;
            // portal.config(hostName.c_str(), "123456789AP");
            config.apid = hostName + "-" + String(GET_CHIPID(), HEX);
            config.password = apPass;
            config.psk = apPass;
            //config.hostName = hostName;//hostnameElm.value+ "-" + String(GET_CHIPID(), HEX);
            // portal.config(hostName.c_str(), "123456789AP");
            Serial.println("hostname set to " + hostName);
        }
        config.homeUri = "/_ac";
        config.apip = ipV;

        portal.on(AUX_MQTTSETTING, loadParams);
        portal.on(AUX_MQTTSAVE, saveParams);
    }
    else
    {
        Serial.println("aux. load error");
    }
    //config.homeUri = "/_ac";
    config.apip = ipV;
    config.autoReconnect = true;
    config.reconnectInterval = 1;
    Serial.print("AP: ");
    Serial.println(hostName);
    Serial.print("Password: ");
    Serial.println(apPass);
    config.title = "Smart SmartTemperatureController Device"; //set title of webapp

    //add different tabs on homepage
    portal.append("/Test", "Test");
    server.on("/Test", testHandler);
    server.on("/", handleRoot);
    server.on("/relayHandler", relayHandler);
    // Starts user web site included the AutoConnect portal.

    // config.auth = AC_AUTH_DIGEST;
    // config.authScope = AC_AUTHSCOPE_PARTIAL;
    // config.username = hostName;
    // config.password = settingsPass;
    config.autoReconnect = true;  // Attempt automatic reconnection.
    config.reconnectInterval = 3; // Seek interval time is 180[s].

    portal.config(config);
    portal.whileCaptivePortal(whileCP);
    portal.onDetect(atDetect);
    // portal.load(FPSTR(PAGE_AUTH));

    // portal.disableMenu(AC_MENUITEM_DISCONNECT);
    // portal.disableMenu(AC_MENUITEM_OPENSSIDS);
    // portal.disableMenu(AC_MENUITEM_CONFIGNEW);
    if (portal.begin())
    {
        Serial.println("Started, IP:" + WiFi.localIP().toString());
        LcdPrint("IP: ", WiFi.localIP().toString());
        ledState(AP_MODE);
    }
    else
    {
        Serial.println("Connection failed.");
        while (true)
        {
            yield();
            delay(100);
        }
    }

    MDNS.addService("http", "tcp", 80);
    mqttConnect(); //start mqtt
}
int lcdPage = 0;
void loop()
{
    //don't edit this function
    //use whileCP function above as a main loop
    server.handleClient();

    portal.handleRequest();
    loopZCD();
    loopHyst();

    if (n2.repeat()) //publish data to mqtt server
    {
        if (lcdPage == 0)
        {
            ledState(ACTIVE_MODE);
            LcdPrint("Status: ", "Active/STA");
            Serial.println("Status: Active/STA");
            lcdPage++;
        }
        else if (lcdPage == 1)
        {
            LcdPrint("SensorTemp: ", getTempStr());
            Serial.print("SensorTemp: ");
            Serial.println(getTempStr());
            lcdPage++;
        }
        else if (lcdPage == 2)
        {
            postReq();
            LcdPrint("CmotsTemp: ", TempL);
            Serial.print("CmotsTemp: ");
            Serial.println(TempL);
            lcdPage++;
        }
        else if (lcdPage == 3)
        {

            LcdPrint("SetPoint: ", String(getSetPoint()));
            Serial.print("SetPoint: ");
            Serial.println(String(getSetPoint()));
            lcdPage++;
        }
        else if (lcdPage == 4)
        {

            LcdPrint("IP: ", WiFi.localIP().toString());
            Serial.print("IP: ");
            Serial.println(WiFi.localIP().toString());
            lcdPage++;
        }
        else if (lcdPage == 5)
        {

            LcdPrint("Address: ", "http://cmots.local");
            Serial.println("Address: http://cmots.local");
            lcdPage = 0;
        }
    }
    // if (n1.repeat())
    // {
    //     LcdPrint("SesnorTemp: ", getTempStr());
    // }
    // if (n3.repeat())
    // {
    //     postReq();
    //     LcdPrint("CmotsTemp: ", TempL);
    //     // Serial.println(getFrequency());
    // }
    // if (n4.repeat())
    // {
    //     LcdPrint("SetPoint: ", String(getSetPoint()));
    // }
}