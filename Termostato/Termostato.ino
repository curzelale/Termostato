#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WiFiMulti.h> 
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include "DHT.h" 

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define DHTPIN 5
ESP8266WiFiMulti wifiMulti;

ESP8266WebServer server(80);

Adafruit_SSD1306 display(DISPLAY_WIDTH, DISPLAY_HEIGHT);

int targetTemp = 20;
float humidity = 0;
float curTemp = 0.0;
bool heaterStatus = false;  
DHT dht(DHTPIN, DHT11);


void handleRoot();

void tempUp();
void tempDown();

void printText(char *text){
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println(text);
  display.display();
}

void setup(){
  Serial.begin(9600);
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  printText("Connessione WiFi");
  wifiMulti.addAP("NETGEAR34_EXT", "cleverhippo630");
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(1000);
    printText("Errore, ritento");
  }
  dht.begin();

  MDNS.begin("esp8266");
  printText("Connesso");
  server.on("/", HTTP_GET, handleRoot);
  server.on("/TEMPUP", HTTP_GET, tempUp);
  server.on("/TEMPDOWN", HTTP_GET, tempDown);
  server.begin();
  Serial.println(WiFi.localIP());
}

void loop(){
  server.handleClient();
  int curTemp = dht.readTemperature();
  int humidity = dht.readHumidity();
  delay(500);
}


void handleRoot(){
  String resJson= "{'temperature': ";
  resJson.concat(curTemp);
  resJson.concat(",'humidity': ");
  resJson.concat(humidity);
  resJson.concat(",'targetTemperature': ");
  resJson.concat(targetTemp);
  resJson.concat(",'heaterOn': ");
  resJson.concat(heaterStatus);
  resJson.concat("}");
  server.send(200, "application/json", resJson);
}

void tempUp(){
  targetTemp ++;
  String res  = "{'targetTemp':";
  res.concat(targetTemp);
  res.concat("}");
  server.send(200, "application/json", res);
}

void tempDown(){
  targetTemp --;
  String res  = "{'targetTemp':";
  res.concat(targetTemp);
  res.concat("}");
  server.send(200, "application/json", res);
}

