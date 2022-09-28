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
  server.serveStatic("/config.html", SPIFFS, "/config.html");

  server.onNotFound([](){
    server.send(404, "text/plain", "404: Not Found");
  });

  server.on("/readTemperature",[](){
    DynamicJsonDocument doc(BUFFER_MAX);
    char buf[BUFFER_MAX];
    double val;

    for(int i=0;i<N_PROBES;i++){
      val= getTemperature(getResistance(i),coeffs[i]);
      if(server.arg("unit") == "c") val = ktoc(val);
      else if(server.arg("unit") == "f") val = ktof(val);
      doc[i] = val;
    }
    
    serializeJson(doc, buf);
    server.send(200, "text/json", buf);
  });

  server.on("/readResistance",[](){
    DynamicJsonDocument doc(BUFFER_MAX);
    char buf[BUFFER_MAX];
    int probe = server.arg("probe").toInt()-1;
    
    if(probe > -1 && probe < N_PROBES){
      doc[0] = getResistance(probe);
    }
    
    serializeJson(doc, buf);
    server.send(200, "text/json", buf);
  });

  server.on("/saveCoeffs",[](){
    DynamicJsonDocument doc(BUFFER_MAX);
    char buf[BUFFER_MAX];

    deserializeJson(doc,server.arg("points"));
    int probe = server.arg("probe").toInt()-1;

    getCoefficients(
      ctok(doc[0]["temperature"].as<double>()),
      ctok(doc[1]["temperature"].as<double>()),
      ctok(doc[2]["temperature"].as<double>()),
      doc[0]["resistance"].as<double>(),
      doc[1]["resistance"].as<double>(),
      doc[2]["resistance"].as<double>(),
      coeffs[probe]
    );

    doc.clear();

    doc["a"] = coeffs[probe][0];
    doc["b"] = coeffs[probe][1];
    doc["c"] = coeffs[probe][2];

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
