#include <Arduino.h>
#include <ArduinoJson.h>
#include <ESP8266mDNS.h>
#include <ESP8266WebServer.h>
#include <uri/UriBraces.h>
#include "http-server.h"
#include "command.h"
#include "config.h"

extern ESP8266WebServer httpRestServer;
extern int RELAYS[];

void setupWifi()
{
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  Serial.print("Connection to ");
  Serial.println(WIFI_SSID);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(250);
    Serial.print(".");
  }

  Serial.println();
  Serial.print("Connected to: ");
  Serial.println(WiFi.SSID());
  Serial.print("IP dddress: ");
  Serial.println(WiFi.localIP());
}

void setupMdns()
{
  if (!MDNS.begin("relay-bridge"))
  {
    Serial.println("Error setting up mDNS responder!");
    return;
  }

  MDNS.addService("http", "tcp", 80);
  Serial.println("mDNS responder started: home-relay.local");
}

void getHelloWord()
{
  httpRestServer.send(200, F("application/json"), "{\"name\": \"Hello world\"}");
}

void handleNotFound()
{
  httpRestServer.send(404);
}

void handleStatus()
{
  const short relayCount = 4;
  const size_t capacity = JSON_ARRAY_SIZE(relayCount) + (relayCount * JSON_OBJECT_SIZE(2));
  StaticJsonDocument<capacity> doc;

  // create an empty array
  JsonArray array = doc.to<JsonArray>();

  for (int i = 0; i <= 4 - 1; i++)
  {
    bool on = digitalRead(RELAYS[i]) == LOW;
    JsonObject nested = array.createNestedObject();

    nested["relay"] = i;
    nested["ison"] = on;
  }

  serializeJson(doc, Serial);
  String output;
  serializeJson(doc, output);
  httpRestServer.send(200, F("application/json"), output);
}

void respondIsOn(bool on)
{
  String status = on ? "true" : "false";
  httpRestServer.send(200, F("application/json"), "{\"ison\":" + status + "}");
}

void handleRelayStatus()
{
  String sRelay = httpRestServer.pathArg(0);
  if (sRelay == "")
  {
    httpRestServer.send(404);
  }

  short relay = atoi(sRelay.c_str());
  respondIsOn((digitalRead(RELAYS[relay]) == LOW));
}

void handleRelayCommand()
{
  String sRelay = httpRestServer.pathArg(0);
  if (sRelay == "")
  {
    httpRestServer.send(404);
  }

  short relay = atoi(sRelay.c_str());
  String postBody = httpRestServer.arg("plain");

  DynamicJsonDocument json(512);
  DeserializationError error = deserializeJson(json, postBody);
  if (error)
  {
    String msg = error.c_str();

    Serial.print(F("Error parsing JSON "));
    Serial.println(msg);

    httpRestServer.send(400, F("application/json"), "{\"error\":\"" + msg + "\"}");
    return;
  }

  // Validation
  try
  {
    Command cmd = parseRelayCommand(relay, json);
    bool isOn = handleCommand(cmd);
    respondIsOn(isOn);
  }
  catch (const char *e)
  {
    Serial.println(e);
    httpRestServer.send(400, F("application/json"), "{\"error\":\"" + String(e) + "\"}");
  }
}

void setupRoutes()
{
  httpRestServer.on("/", HTTP_GET, getHelloWord);
  httpRestServer.on(UriBraces(F("/relay/{}")), HTTP_GET, handleRelayStatus);
  httpRestServer.on(UriBraces(F("/relay/{}")), HTTP_POST, handleRelayCommand);
  httpRestServer.on("/status", HTTP_GET, handleStatus);
  httpRestServer.onNotFound(handleNotFound);
}
