#include <ArduinoJson.h>
#define myserial Serial1 
void ShowSerialData();

 
void setupGPRS()
{
  myserial.begin(9600);        // the GPRS baud rate
  Serial.println("Initializing..........");
  delay(2000);
}
 
void loopGPRS()
{
  
 
 /********************GSM Communication Starts********************/
 
  if (myserial.available())
  {
    Serial.write(myserial.read());
  }
 
  myserial.println("AT");
  delay(3000);
 
  myserial.println("AT+SAPBR=3,1,\"Contype\",\"GPRS\"");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+SAPBR=3,1,\"APN\",\"airtelgprs.com\"");//APN
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+SAPBR=1,1");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+SAPBR=2,1");
  delay(6000);
  ShowSerialData();
 
 
  myserial.println("AT+HTTPINIT");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+HTTPPARA=\"CID\",1");
  delay(6000);
  ShowSerialData();
 
  
  String sendtoserver= "imei="+String(IMEIStr)+String("&email=")+String(emailAddress);
  
  delay(4000);
 
  myserial.println("AT+HTTPPARA=\"URL\",\"http://cmots.ca:8081/api/checkProductInfo"); //Server address
  delay(4000);
  ShowSerialData();
 
  myserial.println("AT+HTTPPARA=\"CONTENT\",\"application/json\"");
  delay(2000);
  ShowSerialData();
 
 
  myserial.println("AT+HTTPDATA=" + String(sendtoserver.length()) + ",100000");
  Serial.println(sendtoserver);
  delay(6000);
  ShowSerialData();
 
  myserial.println(sendtoserver);
  delay(6000);
  ShowSerialData;
  // set http action type 0 = GET, 1 = POST, 2 = HEAD
  myserial.println("AT+HTTPACTION=1");
  delay(6000);
  ShowSerialData();
   // read server response
  myserial.println("AT+HTTPREAD");
  delay(6000);
  ShowSerialData();
 
  myserial.println("AT+HTTPTERM");
  delay(10000);
  ShowSerialData;
 
  /********************GSM Communication Stops********************/
 
}
 
 
void ShowSerialData()
{
  String str="";
  while (myserial.available() != 0){
    str=str+String(myserial.read());
  }
  Serial.println(str);
  if(str.indexOf("{")>=0 && str.indexOf("}")>=0){
    //valid response json arrived
     DynamicJsonDocument doc(2048);
      //Serial.print(http.getString());
      deserializeJson(doc, str);
     StatusL=doc["status"].as<String>();
     if(StatusL.indexOf("true")>=0){
         TempL=doc["userImeiProductData"]["Temperature"].as<String>();
         devList[0]=TempL;
         HumidL=doc["userImeiProductData"]["Humidity"].as<String>();
         PressureL=doc["userImeiProductData"]["Pressure"].as<String>();
         BattL=doc["userImeiProductData"]["Battery"].as<String>();
         ServerTime=doc["userImeiProductData"]["ServerTime"].as<String>();
         LcdPrint("T1:"+String(devList[0]),"T2:"+String(getTempValue()));
         setPointConfig(devList[0].toFloat());
     }
         

  }
  delay(200);
 
}

