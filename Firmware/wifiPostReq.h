

#include <HTTPClient.h>

//Your Domain name with URL path or IP address with path
const char* serverName = "http://cmots.ca:8081/api/checkProductInfo";

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastTime = 0;
// Timer set to 10 minutes (600000)
//unsigned long timerDelay = 600000;
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 5000;
String StatusL="";
String TempL="";
String HumidL="";
String PressureL="";
String BattL="";
String ServerTime="";

void setupWiFiHTTP() {
  
}

void postReq() {
  //Send an HTTP POST request every 10 minutes
  if(emailAddress.length()>0 && IMEIStr.length()>0){
  if ((millis() - lastTime) > timerDelay) {
    //Check WiFi connection status
    if(WiFi.status()== WL_CONNECTED){
      HTTPClient http;
      
      // Your Domain name with URL path or IP address with path
      http.begin(serverName);

      // Specify content-type header
      http.addHeader("Content-Type", "application/x-www-form-urlencoded");
      // Data to send with HTTP POST
      String httpRequestData = "imei="+String(IMEIStr)+String("&email=")+String(emailAddress);           
      // Send HTTP POST request
      IMEIsList[0]=IMEIStr;
      int httpResponseCode = http.POST(httpRequestData);
      
      // If you need an HTTP request with a content type: application/json, use the following:
      //http.addHeader("Content-Type", "application/json");
      //int httpResponseCode = http.POST("{\"api_key\":\"tPmAT5Ab3j7F9\",\"sensor\":\"BME280\",\"value1\":\"24.25\",\"value2\":\"49.54\",\"value3\":\"1005.14\"}");

      // If you need an HTTP request with a content type: text/plain
      //http.addHeader("Content-Type", "text/plain");
      //int httpResponseCode = http.POST("Hello, World!");
     
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
      if(httpResponseCode==200){
      DynamicJsonDocument doc(2048);
      //Serial.print(http.getString());
      deserializeJson(doc, http.getStream());
     StatusL=doc["status"].as<String>();
     if(StatusL.indexOf("true")>=0){
         TempL=doc["userImeiProductData"]["Temperature"].as<String>();
         devList[0]=TempL;
         HumidL=doc["userImeiProductData"]["Humidity"].as<String>();
         PressureL=doc["userImeiProductData"]["Pressure"].as<String>();
         BattL=doc["userImeiProductData"]["Battery"].as<String>();
         ServerTime=doc["userImeiProductData"]["ServerTime"].as<String>();
     }
         
     }
     else{
         //for future use
     }
      
    Serial.println(TempL);
    Serial.println(HumidL);
    Serial.println(PressureL);
    Serial.println(BattL);
    Serial.println(ServerTime);

      
        
      // Free resources
      http.end();
    }
    else {
      Serial.println("WiFi Disconnected");
    }
    lastTime = millis();
  }
  }
}