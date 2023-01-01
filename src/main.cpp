#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <ArduinoOTA.h>
#include "http-server.h"
#include "config.h"

#define DEBUG true

ESP8266WebServer httpRestServer(80);

int RELAYS[] = {D5, D6, D7, D8};

void setupPins()
{
  for (int i = sizeof(RELAYS) - 1; i >= 0; i--)
  {
    pinMode(RELAYS[i], OUTPUT);
    // Normal open configuration
    digitalWrite(RELAYS[i], HIGH);
  }
}

void setupOTA()
{
  #if defined(OTA_PASS)
  ArduinoOTA.setPassword(OTA_PASS);
  #endif // OTA_PASS
  
  
  ArduinoOTA.onStart([]()
                     { Serial.println("Start"); });
  ArduinoOTA.onEnd([]()
                   { Serial.println("\nEnd"); });
  ArduinoOTA.onProgress([](unsigned int progress, unsigned int total)
                        { Serial.printf("Progress: %u%%\r", (progress / (total / 100))); });
  ArduinoOTA.onError([](ota_error_t error)
                     {
    Serial.printf("Error[%u]: ", error);
    if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
    else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
    else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
    else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
    else if (error == OTA_END_ERROR) Serial.println("End Failed"); });
}

void setup()
{
  Serial.begin(115200);
  Serial.println("Home Relay");

  setupPins();
  setupWifi();
  setupMdns();
  setupOTA();
  setupRoutes();
  httpRestServer.begin();

  Serial.println("Server up and running");
}

void loop()
{
  ArduinoOTA.handle();
  httpRestServer.handleClient();
}