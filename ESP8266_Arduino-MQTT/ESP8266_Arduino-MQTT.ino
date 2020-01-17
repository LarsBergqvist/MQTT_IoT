//
// This is a minimal example on MQTT publish and subscribe
// from an ESP8266 board to an MQTT broker (I have used a local Mosquitto running on a Raspberry Pi)
// This example uses the Arduino-MQTT library (https://github.com/256dpi/arduino-mqtt)
// Install it in the Arduino IDE before compiling the sketch

#include <ESP8266WiFi.h>
#include <MQTTClient.h>
#include "WIFI_and_broker_parameters.h"

#define CLIENT_NAME "ESP8266_1" // just a name to identify this client
WiFiClient net;
MQTTClient client;

void connect();

void setup() 
{
  Serial.begin(9600);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  client.begin(BROKER_IP,1883, net);

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
  while (!client.connect(CLIENT_NAME)) 
  {
    Serial.print(".");
    delay(1000);
  }
  Serial.println("Connected to broker!");

  client.subscribe("InterestingTopics/#");
}

#define MILLISEC_DELAY_BETWEEN_PUBLISH 10000
unsigned long lastTime = 0;
unsigned long currentValue = 1;

void loop() 
{
  client.loop();

  if (!client.connected()) 
  {
    connectToWiFiAndBroker();
  }

  if(millis() - lastTime > MILLISEC_DELAY_BETWEEN_PUBLISH) 
  {
    lastTime = millis();
    String stringValue = String(currentValue++);
    Serial.println("Publishing a new value");
    Serial.println(stringValue);
    client.publish("ESP8266/CurrentValue", stringValue);
  }
}

void messageReceived(String topic, String payload, char * bytes, unsigned int length) 
{
  Serial.println("Message received: ");
  Serial.println(topic);
  Serial.println(payload);
}
