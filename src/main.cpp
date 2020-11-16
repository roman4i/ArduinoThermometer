#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP085.h>

Adafruit_BMP085 tSens;


// Replace with your network credentials
const char* ssid = "REPLACE_WITH_YOUR_SSID";
const char* password = "REPLACE_WITH_YOUR_PASSWORD";


// Create AsyncWebServer object on port 80
AsyncWebServer server(80);


const char index_html[] PROGMEM = R"rawliteral(
<html>
    <head>
        <title>Temperature checker</title>
        <style>
            .mainCont{
                background-color: aqua;
            }
            .headText{
                text-align: center;
                font-size: 30px;
                font-family: 'Times New Roman', Times, serif;
            }
            .btnReload{
                background-color: burlywood;
            }
        </style>
    </head>
    <body>
        <div class="mainCont">
            <div class="headText"></div>
            <div id="temperature">%TEMPERATURE%</div>
            <div> <a class="btnReload" onclick="reloadT()">Reload</a> </div>
        </div>
    </body>
    <script>
        function reloadT(){
            var xhttp = new XMLHttpRequest();
            xhttp.onreadystatechange = function() {
                if (this.readyState == 4 && this.status == 200) {
                    document.getElementById("temperature").innerHTML = this.responseText + "C";
                }
            };
            xhttp.open("GET", "/temperature", true);
            xhttp.send();
        }
    </script>
</html>)rawliteral";

int getTemp(){
  return tSens.readTemperature();
}

String processor(const String& var){
  if(var == "TEMPERATURE"){
    return String(getTemp());
  }
  return String();
}

void setup(){
  // Serial port for debugging purposes
  Serial.begin(115200);
  if (!tSens.begin()) {
    Serial.println("Could not find a valid BMP185 sensor, check wiring!");
    while (1) {}
  }
  
  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println(".");
  }

  // Print ESP8266 Local IP Address
  Serial.println(WiFi.localIP());

  // Route for root / web page
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/html", index_html, processor);
  });
  server.on("/temperature", HTTP_GET, [](AsyncWebServerRequest *request){
    request->send_P(200, "text/plain", String(getTemp()).c_str());
  });

  // Start server
  server.begin();
}
 
void loop(){  
}