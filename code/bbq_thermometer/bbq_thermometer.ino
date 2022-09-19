#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "wifi_config.h"
#include "utils.h"
#include "probe.h"
#include "network.h"


ESP8266WebServer server(80);
const int BUFFER_MAX = 1000;
const int N_PROBES = 2;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, LOW);
  
  initNetwork(WIFI_SSID,WIFI_PASSWORD,HOSTNAME);
  initAds();

  SPIFFS.begin();
  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/grill-outline.png", SPIFFS, "/grill-outline.png");
  server.serveStatic("/Poppins-Regular.ttf", SPIFFS, "/Poppins-Regular.ttf");
  server.serveStatic("/app.js", SPIFFS, "/app.js");
  server.serveStatic("/probes.html", SPIFFS, "/probes.html");

  server.onNotFound([](){
    server.send(404, "text/plain", "404: Not Found");
  });

  server.on("/read",[](){
    DynamicJsonDocument doc(BUFFER_MAX);
    char buf[BUFFER_MAX];
    double res;

    for(int i=0;i<N_PROBES;i++){
      res = getResistance(i);
      res = ktoc(getTemperature(res,coeffs));
      doc[i] = res;
    }

    serializeJson(doc, buf);
    server.send(200, "text/json", buf);
  });

  server.begin();
  Serial.println("HTTP server started");
  digitalWrite(LED_BUILTIN, HIGH);
}

void loop() {
  MDNS.update();
  server.handleClient();
}
