#include "arduino_secrets.h"
#include <OneWire.h>
#include <DallasTemperature.h>
#include "discord.h"
#include "ESP8266WebServer.h"

#define ONE_WIRE_BUS 4
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
ESP8266WebServer server;
//################# Customizable #################

int coldTemp = 45;
int hotTemp = 50;
int alertTemp = 70;
int tempSensorOffset = -3;

//################################################
int currentTemperature = 0;
bool alert = false;
bool coldTriggered = true;
unsigned long previousMillis = 0;
unsigned long interval = 5000;

void setup() {
  Serial.begin(9600);
  String newHostname = "ESP-temperature-bot";
  WiFi.hostname(newHostname.c_str());
  
  while(WiFi.status()!=WL_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  connectWIFI();
  Serial.println("");
  Serial.print("IP Adress: ");
  Serial.println(WiFi.localIP());
  server.on("/",[](){server.send(200,"text/plain",String(currentTemperature));});
  server.begin();
  sensors.begin();
}

void loop() {
  unsigned long currentMillis = millis();
  server.handleClient();
  if (currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;
      sensors.requestTemperatures();
      currentTemperature = sensors.getTempCByIndex(0) + tempSensorOffset;
      Serial.println(currentTemperature);
      // Temperature logic
        if (currentTemperature > alertTemp && alert == false) {
          sendDiscord("Temperature too high! Alert!");
          alert = true;
        }
        else if (currentTemperature < coldTemp && coldTriggered == false) {
          sendDiscord("Add more fuel!");
          coldTriggered = true;
          alert = false;
        } else if (currentTemperature > hotTemp && coldTriggered == true) {
          coldTriggered = false;
        }
      // End
    }
  }
