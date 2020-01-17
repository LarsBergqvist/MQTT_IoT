//
// This is a minimal example on MQTT publish and subscribe from an ESP8266 board
// to an MQTT broker (I have used a local Mosquitto running on a Raspberry Pi)
// This example uses the PubSub client library (https://github.com/knolleary/pubsubclient)
// Install it in the Arduino IDE before compiling the sketch

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WIFI_and_broker_parameters.h"

#define CLIENT_NAME "ESP8266_1" // just a name to identify this client
WiFiClient wifiClient;
PubSubClient mqttClient(BROKER_IP,BROKER_PORT,wifiClient);

void setup() 
{
  Serial.begin(9600);
  
  mqttClient.setCallback(callback);

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  connectToWiFiAndBroker();
}

void connectToWiFiAndBroker() 
{
  Serial.print("Connecting to WIFI");
  while (WiFi.status() != WL_CONNECTED) 
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to WIFI!");

  Serial.println("Connecting to broker");
  while (!mqttClient.connect(CLIENT_NAME)) 
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to broker!");

  mqttClient.subscribe("InterestingTopics/#");
}

#define MILLISEC_DELAY_BETWEEN_PUBLISH 10000
unsigned long lastTime = 0;
unsigned long currentValue = 1;
char msg[50];

void loop() 
{
  if (!mqttClient.connected()) 
  {
    connectToWiFiAndBroker();
  }

  mqttClient.loop();
  
  if(millis() - lastTime > MILLISEC_DELAY_BETWEEN_PUBLISH) 
  {
    lastTime = millis();
    String stringValue = String(currentValue++);
    Serial.println("Publishing a new value");
    snprintf (msg, 75, "%ld", currentValue);
    Serial.println(currentValue);
    mqttClient.publish("ESP8266/CurrentValue", msg);
  }
}

void callback(char* topic, byte* payload, unsigned int length) 
{
  Serial.println("Message received: ");
  Serial.println(topic);
  for (int i = 0; i < length; i++) 
  {
    Serial.print((char)payload[i]);
  }
  Serial.println("");
  
}


