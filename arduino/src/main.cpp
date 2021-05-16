#include <Arduino.h>
#include <esp8266wifi.h>
#include <esp8266httpclient.h>
#include <string.h>
#include <ArduinoJson.h>
#include <StreamUtils.h>

#define URL "http://pc-ema.lan.com:8000/horno/"
#define DEBUG
#define DEFAULT_CONFIG "{\"samples\": 100, \"analog_reference\": 3.3, \"max_temperature\": 230, \"min_temperature\": 80, \"max_voltage\": 1.77, \"min_voltage\": 1.16}"

const char* ssid = "AsusRedT";
const char* password = "SRME61639295mssd";
WiFiClient client;

StaticJsonDocument<200> extra_data;

void print_debug_message(const char *message)
{
  #ifdef DEBUG 
    Serial.println("DEBUG - " + String(message));
  #endif
}

void update_temperature(int temperature)
{
  HTTPClient http; 
  int response_code;
  String payload = "{\"temperature\": ";
  payload += temperature;
  payload += "}";

  if (http.begin(client, URL))
  {
    print_debug_message("Updating temperature...");
    response_code = http.PATCH(payload);
    if (response_code == HTTP_CODE_OK)
      print_debug_message("OK.");
    else
      print_debug_message("DEBUG - Error updating temperature.");
    http.end();
  }
}

void update_extra_data()
{
  HTTPClient http;
  int response_code;
  if (http.begin(client, URL))
  {
    print_debug_message("Getting information...");
    response_code = http.GET();
    if (response_code == HTTP_CODE_OK)
    {
      print_debug_message("OK.");
      deserializeJson(extra_data, http.getStream());
    }
    else
      print_debug_message("DEBUG - Error updating temperature.");
    http.end();
  }
}

float read_voltage() {
  unsigned long mean = 0;
  float voltage = 0;
  int samples = int(extra_data["samples"]);
  float analog_reference = float(extra_data["analog_reference"]);
  for (int i=0; i < samples; i++) {       
    mean += analogRead(A0);
  }
  mean = mean / samples;                 
  voltage = mean * analog_reference / 1024.0;
  return voltage;
}

int get_temperature_from_voltage(float voltage)
{
  int temperature = 0;
  int max_temperature = int(extra_data["max_temperature"]);
  int min_temperature = int(extra_data["min_temperature"]);
  float max_voltage = float(extra_data["max_voltage"]);
  float min_voltage = float(extra_data["min_voltage"]);
  float slope = (max_temperature - min_temperature)/(max_voltage - min_voltage);
  float intercept = min_temperature - slope * min_voltage;
  temperature = (int)slope * voltage + intercept;
  return temperature;
}

void setup() 
{
  deserializeJson(extra_data, DEFAULT_CONFIG);
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  while (WiFi.status() != WL_CONNECTED) 
  {
    delay(1000);
    Serial.print("Connecting..");
  }
  digitalWrite(LED_BUILTIN, LOW);
}

void loop() 
{
  if (WiFi.status() == WL_CONNECTED) 
  { 
    digitalWrite(LED_BUILTIN, LOW);
    update_extra_data();
    update_temperature(get_temperature_from_voltage(read_voltage()));
  }
  else 
    digitalWrite(LED_BUILTIN, HIGH);
  delay(2000);  
}

