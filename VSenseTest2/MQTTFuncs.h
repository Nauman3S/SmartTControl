void MQTTUnSubscribe();
void MQTTSubscriptions();
void callback(char* topic, byte* payload, unsigned int length);
void reconnect();
bool mqttConnect();
void mqttPublish(String path,String msg) ;

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
      LcdPrint("T1:"+String(devList[0]),"T2:"+String(getTempValue()));
      setPointConfig(devList[0].toFloat());
      }
      if(nD>1){
        for (int i=0;i<nD;i++){
            String IMEIs=ss.StringSeparator(pLoad,';',0);
            String Temps=ss.StringSeparator(pLoad,';',1);

            devList[i]=ss.StringSeparator(Temps,',',i);

            Serial.println(devList[i]);
        

            IMEIsList[i]=ss.StringSeparator(IMEIs,',',i);
            Serial.println(IMEIsList[i]);
            LcdPrint("T1:"+String(devList[0]),"T2:"+String(getTempValue()));
            setPointConfig(devList[0].toFloat());
        //IMEIsList[1]=ss.StringSeparator(IMEIs,',',1);

        }
        
      }


      
      // dev1=ss.StringSeparator(pLoad,';',0);
      // dev2=ss.StringSeparator(pLoad,';',1);
      // Serial.println(dev1);
      // Serial.println(dev2);
     // changeRelayState(compareValues(devList[0].toFloat(),getTempValue()));

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
    internetStatus="Connecting...";

    for (uint8_t i = 0; i < 8; i++) {
      clientId[i] = alphanum[random(62)];
    }
    clientId[8] = '\0';

    if (mqttClient.connect(clientId, mqtt_user, mqtt_pass)) {
      Serial.println("Established:" + String(clientId));
      internetStatus="Connected";
      //mqttClient.subscribe("SmartTControl/data/v");
      MQTTSubscriptions();
      return true;
    }
    else {
      Serial.println("Connection failed:" + String(mqttClient.state()));
      internetStatus="Not-Connected. Retrying...";
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