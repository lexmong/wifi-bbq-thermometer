#include <ArduinoJson.h>
#include <ESP8266WebServer.h>
#include <FS.h>
#include "wifi_config.h"
#include "utils.h"
#include "probe.h"
#include "network.h"

ESP8266WebServer server(80);
const int BUFFER_MAX = 1000;

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
  server.serveStatic("/style.css", SPIFFS, "/style.css");

  if(loadCoefficients("/coeffs.json")){
    Serial.println("Coefficients loaded");
    for(int i =0; i< N_PROBES; i++){
      Serial.print("    probe ");
      Serial.println(i+1);
      for(int j=0;j<3;j++){
        Serial.print("        ");
        Serial.println(coeffs[i][j],10);
      }
    }
  }
  else Serial.println("Failed to load coefficents");

  server.onNotFound([](){
    server.send(404, "text/plain", "404: Not Found");
  });

  /**
   * Read temperature
   * GET args: unit ('c','f')
   */
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

  /**
   * Read resistance
   * GET args: probe (probe id 0 indexed)
   */
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

  /**
   * Calculates and returns coefficients 
   * GET args: 
   * points: JSON [{'temperature'=>t,'resistance'=>},{'temperature'=>t,'resistance'=>},...] 
   * probe: (probe id 0 indexed)
   */
  server.on("/saveCoeffs",[](){
    DynamicJsonDocument doc(BUFFER_MAX);
    char buf[BUFFER_MAX];

    deserializeJson(doc,server.arg("points"));
    int probe = server.arg("probe").toInt()-1;

    //TODO fahrenheit
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

    saveCoefficients();

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

/***
 * Load coefficients from JSON file
 */
bool loadCoefficients(char* filename) {
  StaticJsonDocument<1024> doc;
  File f;
  
  f = SPIFFS.open(filename, "r");
  if(!f)return false;

 
  if(deserializeJson(doc, f)){
    f.close();
    return false;
  }

  for(int i =0; i< N_PROBES; i++){
    coeffs[i][0] = doc[i]["a"].as<double>();
    coeffs[i][1] = doc[i]["b"].as<double>();
    coeffs[i][2] = doc[i]["c"].as<double>();
  }

  f.close();
  return true;
}

/**
 * Save coefficients to JSON file
 */
bool saveCoefficients() {
  DynamicJsonDocument doc(BUFFER_MAX);
  File f;
  
  f = SPIFFS.open("/coeffs.json", "w");
  if(!f)return false;

  for(int i =0;i<N_PROBES;i++){
    doc[i]["a"] = coeffs[i][0];
    doc[i]["b"] = coeffs[i][1];
    doc[i]["c"] = coeffs[i][2];
  }

  serializeJson(doc,f);
  f.close();
  return false;
}
