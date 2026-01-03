#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>

#define VOLT_PIN 34
#define CURR_PIN 35

float Vrms = 0, Irms = 0, realPower = 0, apparentPower = 0, pf = 0, freq = 50;

WebServer server(80);

const char* ssid = "YOUR_WIFI";
const char* password = "YOUR_PASS";

void handleData(){
  DynamicJsonDocument doc(512);

  doc["Vrms"] = Vrms;
  doc["Irms"] = Irms;
  doc["realPower"] = realPower;
  doc["apparentPower"] = apparentPower;
  doc["pf"] = pf;
  doc["freq"] = freq;

  String out;
  serializeJson(doc, out);
  server.send(200, "application/json", out);
}

void setup(){
  Serial.begin(115200);

  WiFi.begin(ssid, password);
  while(WiFi.status()!=WL_CONNECTED){
    delay(500);
    Serial.print(".");
  }

  Serial.println(WiFi.localIP());

  server.on("/data", handleData);
  server.begin();
}

void loop(){
  // ---- Simple RMS Estimation Demo ----
  float sumV = 0, sumI = 0;
  int samples = 1000;

  for(int i=0;i<samples;i++){
    float v = analogRead(VOLT_PIN);
    float c = analogRead(CURR_PIN);

    sumV += v*v;
    sumI += c*c;
    delayMicroseconds(200);
  }

  Vrms = sqrt(sumV/samples);
  Irms = sqrt(sumI/samples);

  apparentPower = Vrms * Irms;
  realPower = apparentPower * 0.9; // Assume PF~0.9 initially
  pf = realPower / (apparentPower+0.0001);

  server.handleClient();
}
