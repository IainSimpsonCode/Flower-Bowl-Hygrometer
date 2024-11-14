#include "DHT.h"
#include <WiFi.h>
#include <WebServer.h>

#define DHTPIN 15     // Digital pin connected to the DHT sensor
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321

DHT dht(DHTPIN, DHTTYPE);

float Temp = 0;
float RH = 0;
float DT = 0;

/* Put your SSID & Password */
const char* ssid = "ESP32";  // Enter SSID here
const char* password = "FBIce2024";  //Enter Password here

/* Put IP Address details */
IPAddress local_ip(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

WebServer server(80);

void setup() {
  Serial.begin(115200);

  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_ip, gateway, subnet);
  delay(100);

  server.on("/", handle_OnConnect);
  server.on("/debug", handle_HTMLDebug);
  server.on("/query", handle_OnConnect);
  server.on("/get/temperature", handle_Temp);
  server.on("/get/humidity", handle_RH);
  server.on("/get/dewpoint", handle_DT);
  server.onNotFound(handle_NotFound);
  
  server.begin();
  dht.begin();

  pinMode(2, HIGH);
}

void loop() {
  server.handleClient();
}

void getTemperature(){
  // Reading temperature or humidity takes about 250 milliseconds!
  // Sensor readings may also be up to 2 seconds 'old' (its a very slow sensor)
  RH = dht.readHumidity();
  // Read temperature as Celsius (the default)
  Temp = dht.readTemperature();
  //Calculate dew point
  DT = Temp - ((100 - RH) / 5);

  Serial.println("Temp: " + String(Temp, 2));
  Serial.println("RH: " + String(RH, 2));
  Serial.println("DT: " + String(DT, 2));
}

void handle_OnConnect() {
  server.send(200, "text/html", SendHTML()); 
}

void handle_HTMLDebug() {
  server.send(200, "text/plain", SendHTML()); 
}

void handle_Temp() {
  getTemperature();
  server.send(300, "text/plain", String(Temp));
}

void handle_RH() {
  getTemperature();
  server.send(300, "text/plain", String(RH));
}

void handle_DT() {
  getTemperature();
  server.send(300, "text/plain", String(DT));
}

void handle_NotFound(){
  server.send(404, "text/plain", "Not found");
}

String SendHTML(){
  getTemperature();
  
  String htmlReturn = "<!DOCTYPE html><html><head><title>Flower Bowl Ice Temps Display</title><meta charset=\"utf-8\"><meta name=\"viewport\" content=\"width=device-width, initial-scale=1\"><style>* {box-sizing: border-box;}body {font-family: Arial, Helvetica, sans-serif;}/* Style the header */header {background-color: #690b8b;padding: 30px;text-align: center;font-size: 3vw;color: white;}article {float: left;padding: 20px;width: 100%;background-color: #f1f1f1;}/* Clear floats after the columns */section::after {content: "";display: table;clear: both;}/* Style the footer */footer {background-color: #777;padding: 10px;text-align: center;color: white;}</style></head><body><header><h2>Curling Hall Temps</h2></header><section><article><p>Air Temperature = ";
  htmlReturn += Temp; 
  htmlReturn += "</p><p>Relative Humidity = ";
  htmlReturn += RH;
  htmlReturn += "</p><p>Dew Point = ";
  htmlReturn += DT;
  htmlReturn += "</p><button onClick=\"window.location.reload();\">Refresh Page</button></article></section><footer><p>192.168.1.1</p></footer></body></html>";

  return htmlReturn;
}
