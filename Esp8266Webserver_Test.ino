/*********
  Rui Santos
  Complete project details at https://randomnerdtutorials.com/esp8266-nodemcu-access-point-ap-web-server/
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.
*********/

// Import required libraries
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Hash.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>



const char* ssid     = "Lady Elizabeth";
const char* password = "123456789";


// current pressures, updated in loop()
float p0 = 0.0;
float p1 = 0.0;
float p2 = 0.0;
float p3 = 0.0;
float he_0 = 0;
float he_1 = 0; 
String he_0_string; 
String he_1_string; 


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);

// Generally, you should use "unsigned long" for variables that hold time
// The value will quickly become too large for an int to store
unsigned long previousMillis = 0;    // will store last time DHT was updated

// Updates DHT readings every 5 seconds
const long interval = 5000;  

const char index_html[] PROGMEM = R"rawliteral(
<!DOCTYPE HTML><html>
<head>
  <meta name="viewport" content="width=device-width, initial-scale=1">
  <style>
    html {
     font-family: Arial;
     display: inline-block;
     margin: 0px auto;
     text-align: center;
    }
    h2 { font-size: 3.0rem; }
    p { font-size: 3.0rem; }
    .units { font-size: 1.2rem; }
    .sens-labels{font-size: 1.5rem; vertical-align:middle; padding-bottom: 15px;}
  </style>
</head>
<body>
  <h2>LE Onboard Systems</h2>
  <p>
    <span class="sens-labels">HE:</span> 
    <span id="helium">%P0%</span>
    <sup class="units">psi</sup>
  </p>
  <p>
    <span class="sens-labels">CH4:</span>
    <span id="methane">%P1%</span>
    <sup class="units">psi</sup>
  </p>
  <p>
    <span class="sens-labels">LOX:</span>
    <span id="Liquid-Oxygen">%P2%</span>
    <sup class="units">psi</sup>
  </p>
  <p>
    <span class="sens-labels">Engine:</span>
    <span id="Engine">%P3%</span>
    <sup class="units">psi</sup>
  </p>
  <p>
    <span class="sens-labels">Lox Vent:</span>
    <span id="Hall-Effect-Lox">%HE0%</span>
  </p>
  <p>
    <span class="sens-labels">CH4 Vent:</span>
    <span id="Hall-Effect-CH4">%HE1%</span>
  </p>
</body>
<script>
setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("helium").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/p0", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("methane").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/p1", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("Liquid-Oxygen").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/p2", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("Engine").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/p3", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("Hall-Effect-CH4").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/he0", true);
  xhttp.send();
}, 1000 ) ;

setInterval(function ( ) {
  var xhttp = new XMLHttpRequest();
  xhttp.onreadystatechange = function() {
    if (this.readyState == 4 && this.status == 200) {
      document.getElementById("Hall-Effect-Lox").innerHTML = this.responseText;
    }
  };
  xhttp.open("GET", "/he1", true);
  xhttp.send();
}, 1000 ) ;

</script>
</html>)rawliteral";

// Replaces placeholder with DHT values
String processor(const String& var){
  //Serial.println(var);
  if(var == "P0"){
    return String(p0);
  }
  else if(var == "P1"){
    return String(p1);
  }
  else if(var == "P2"){
    return String(p2);
  }
  else if(var == "P3"){
    return String(p3);
  }
  else if(var == "HE0"){
    return String(he_0_string);
  }
  else if(var == "HE1"){
    return String(he_1_string);
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);

   
  Serial.print("Setting AP (Access Point)…");
  // Remove the password parameter, if you want the AP (Access Point) to be open
  WiFi.softAP(ssid, password);

  IPAddress IP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(IP);

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/p0", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(p0).c_str());
  });
  server.on("/p1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(p1).c_str());
  });
  server.on("/p2", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(p2).c_str());
  });
  server.on("/p3", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(p3).c_str());
  });
  server.on("/he0", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(he_0_string).c_str());
  });
  server.on("/he1", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(he_1_string).c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){  
  if (Serial.available()){
    char inbyte; 
    inbyte = Serial.read(); 
    if (inbyte == '\n'){
      Serial.parseFloat();
      p0 = Serial.parseFloat();
      p1 = Serial.parseFloat();
      p2 = Serial.parseFloat();
      p3 = Serial.parseFloat();
      he_0 = Serial.parseFloat();
      he_1 = Serial.parseFloat();
      if (he_0 > 300.0){
        he_0_string = "closed";
      }
      else{
        he_0_string = "open";
      }
      if (he_1 > 300.0){
        he_1_string = "closed";
      }
      else{
        he_1_string = "open";
      }
    }
  }
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    // save the last time you updated the DHT values
    previousMillis = currentMillis;
    // Read temperature as Celsius (the default)
    //p0 = random(50);
//    p1 = random(50);
//    p2 = random(50);
//    p3 = random(100);   
  }
}
