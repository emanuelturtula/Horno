/*
   Copyright (c) 2015, Majenko Technologies
   All rights reserved.

   Redistribution and use in source and binary forms, with or without modification,
   are permitted provided that the following conditions are met:

 * * Redistributions of source code must retain the above copyright notice, this
     list of conditions and the following disclaimer.

 * * Redistributions in binary form must reproduce the above copyright notice, this
     list of conditions and the following disclaimer in the documentation and/or
     other materials provided with the distribution.

 * * Neither the name of Majenko Technologies nor the names of its
     contributors may be used to endorse or promote products derived from
     this software without specific prior written permission.

   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
   ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
   WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
   DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
   ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
   (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
   LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
   ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

#ifndef STASSID
#define STASSID "AsusRedT"
#define STAPSK  "SRME61639295mssd"
#endif

#define CANT_MUESTRAS 100
#define VALOR_AREF 3.3
#define MAX_TEMP 230
#define MIN_TEMP 80
#define MAX_V 1.77
#define MIN_V 1.16
#define a ((MAX_TEMP - MIN_TEMP)/(MAX_V - MIN_V))
#define b (MIN_TEMP - a*MIN_V)

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

const int led = 2;

void handleRoot() {
  float v = leer_tension();
  int temperatura = leer_temperatura(v);
  char temp[400];
  if (temperatura < 80) {
    snprintf(temp, 400,
    "<html>\
      <head>\
        <meta http-equiv='refresh' content='1'/>\
        <title>Horno</title>\
        <style>\
          body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
        </style>\
      </head>\
      <body>\
        <h1>Temperatura: %s</h1>\
        <p>Tension: %.2f</p>\
      </body>\
    </html>",
            "< 80", v
    );
  }
  else {
    snprintf(temp, 400,
    "<html>\
      <head>\
        <meta http-equiv='refresh' content='1'/>\
        <title>Horno</title>\
        <style>\
          body { background-color: #cccccc; font-family: Arial, Helvetica, Sans-Serif; Color: #000088; }\
        </style>\
      </head>\
      <body>\
        <h1>Temperatura: %02d</h1>\
        <p>Tension: %.2f</p>\
      </body>\
    </html>",
            temperatura, v
    );    
  }

          
  server.send(200, "text/html", temp);
}

void handleNotFound() {
  digitalWrite(led, 1);
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";

  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }

  server.send(404, "text/plain", message);
  digitalWrite(led, 0);
}


void setup(void) {
  pinMode(led, OUTPUT);
  pinMode(A0, INPUT);
  digitalWrite(led, 1);
  Serial.begin(115200);
  
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  
  digitalWrite(led, 0);
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  
  if (MDNS.begin("horno")) {
    Serial.println("MDNS responder started");
  }

  server.on("/", handleRoot);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}

float leer_tension() {
  unsigned long prom = 0;
  float v = 0;
  for (int i=0; i < CANT_MUESTRAS ; i++) {       // Tomo 50 muestras y las sumo
    prom += analogRead(A0);
  }
  prom = prom / CANT_MUESTRAS;                 //Hago un promedio de las muestras
  v = prom * VALOR_AREF / 1024.0;
  return v;
}

int leer_temperatura(float v) {
  int temp = 0;
  temp = (int)a * v + b;
  return temp;    //Calculo la temperatura aproximando la curva real con un polinomio de 3er grado.
}
