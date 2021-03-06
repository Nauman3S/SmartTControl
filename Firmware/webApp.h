void cmotsValues();
void handleRoot();
void sendRedirect(String uri);
void handleGPIO();

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

    page += String(F("<h4>InternetStatus: "));
    page += String((internetStatus));
    page += String(F("</h4><br><br>"));

    page += String(F("<h6>Data From Server Received At: "));
    page += String((ServerTime));
    page += String(F("</h6><br><br>"));

    page += String(F("<h6>Device Details "));
    page += String(F("</h6><br><br>"));

 //   for (int j=0;j<nD;j++){

    page += String(F("<h4>Sensor "));
//    page += String((j));
    page += String("</h4><br>  ");
    page += String(F("<h5"));
    page += String(F("\">IMEI "));
    page += String((IMEIStr));
    page += String(F("</h5><br>"));
    page += String(F("<h5>Temperature: "));
    page += String((TempL));
    page += String(F("</h5><br>"));
    page += String(F("<h5>Humidity: "));
    page += String((HumidL));
    page += String(F("</h5><br>"));
    page += String(F("<h5>Pressure: "));
    page += String((PressureL));
    page += String(F("</h5><br>"));
    page += String(F("<h5>Battery: "));
    page += String((BattL));
    page += String(F("</h5><br><br>"));
    
    
   // }

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
  "<h3>Smart T Controller</h3><br>"
  "");
  // page += String(BUILTIN_LED);
  // page += String(F(") : <span style=\"font-weight:bold;color:"));
  // page += digitalRead(BUILTIN_LED) ? String("Tomato\">HIGH") : String("SlateBlue\">LOW");
  // page += String(F("</span>"));
  // page += String(F("<p>Server State<br><a class=\"button\" href=\"/io?v=low\">STOP</a><a class=\"button\" href=\"/io?v=high\">RUN</a></p>"));


  page += String(F("<h3>Enter your cmots.ca credentials and data freq. below</h3><br><br><form action=/io?v><input type=\"text\" id=\"email\" name=\"email\" placeholder=\"Email Address\"><br><br>    <input placeholder=\"IMEI\" type=\"text\" id=\"pass\" name=\"imei\"><br><br>  <input type=\"submit\" value=\"Connect\"></form><br><br><p>You can visit cmots tab from menu.</p>"));
  page += String(F("<h5>Connected IMEI: "));
  page += String((IMEIStr));
  page += String(F("</h5>"));

   page += String(F("<h4>Connection Mode "));
    
    page += String("</h4><br>  ");
    page += String(F("<a class=\"button\" href=\"/conn?v=wifi"));
    page += String(F("\">WiFi "));
    page += String(F("</a><br>"));
    page += String(F("<a class=\"button\" href=\"/conn?v=gprs"));
    page += String(F("\">GPRS "));
    page += String(F("</a><br>"));
    page += String(F("<h5>SelectedMode: "));
    
    page += String((connectionMode));
    page += String(F("</h5><br><br>"));
   

  page += String(F("</body></html>"));

  
  server.send(200, "text/html", page);
}
void redirectToHome() {
  server.sendHeader("Location", "/", true);
  server.send(302, "text/plain", "");
  //server.client().stop();
}
void sendRedirect(String uri) {
  server.sendHeader("Location", uri, true);
  server.send(302, "text/plain", "");
  server.client().stop();
}
void handleCON() {
  String argV=String(server.arg("v"));
  if(argV=="wifi"){
    connectionMode="WiFi";
  }
  else if(argV=="gprs"){
    connectionMode="GPRS";
  }
  
  redirectToHome();
  
}
void handleDEV() {
  String argV=String(server.arg("v"));
  Serial.println(argV);
  selectedDeviceIndex= argV.toInt();
  redirectToHome();
  
}
void handleGPIO() {
  OLDemailAddress=emailAddress;
  String em=String(server.arg("email"));
  emailAddress=em;
  String imei=String(server.arg("imei"));
  //String freq=String(server.arg("freq"));
  Serial.println(em);
  Serial.println(imei);
  String dataV=String(em);
  dataV+=String(";");
  dataV+=String(imei);
  IMEIStr=imei;
 // mqttPublish("SmartTControl/creds/data",dataV);
  //mqttPublish("SmartTControl/device/freq",freq);
  //mqttPublish("SmartTControl/device/run","3");//to run
  //reconnect();//for new email address subscription
  //MQTTUnSubscribe();
  delay(0.5);
  //MQTTSubscriptions();
  if (server.arg("v") == "low"){
    digitalWrite(BUILTIN_LED, LOW);
    //mqttPublish("SmartTControl/device/run","0");//to off
  }
  else if (server.arg("v") == "high"){
    digitalWrite(BUILTIN_LED, HIGH);
   // mqttPublish("SmartTControl/device/run","1");//to off
    }
   else{
     Serial.println(String(server.arg("v")));
    
   }
   
  sendRedirect("/");
}
