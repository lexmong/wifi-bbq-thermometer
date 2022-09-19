#include <ESP8266WiFi.h>
#include <ESP8266mDNS.h>

void initNetwork(const char* ssid, const char* password, const char* hostName){
  Serial.print("\n\n");
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.hostname(hostName);
  WiFi.begin(ssid, password);
 
  while(WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  MDNS.begin(hostName);

  Serial.println("");
  Serial.print("WiFi connected ");
  Serial.print(WiFi.localIP());
  Serial.print(" / ");
  Serial.print(hostName);
  Serial.println(".local");
}
