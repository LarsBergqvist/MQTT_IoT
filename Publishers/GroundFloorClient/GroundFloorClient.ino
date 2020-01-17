//
// This is an example on MQTT publish from an ESP8266 board
// to an MQTT broker (I have used a local Mosquitto running on a Raspberry Pi)
// This example uses the PubSub client library (https://github.com/knolleary/pubsubclient)
// Install it in the Arduino IDE before compiling the sketch
// Sensor values are fetched from an indoor DHT22 sensor and a soil moisture sensor


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WIFI_and_broker_parameters.h"

//
// WIFI and MQTT setup
//
#define CLIENT_NAME "GroundFloorClient"
WiFiClient wifiClient;
PubSubClient mqttClient(BROKER_IP,BROKER_PORT,wifiClient);

//
// Sensor setup
//
#include <DHT.h>
#define DHTPIN_INDOOR 12
#define DHTTYPE_INDOOR DHT22
DHT dht_indoor(DHTPIN_INDOOR, DHTTYPE_INDOOR);
#define SOILMOISTURE_WARNING_PIN 14


void setup() 
{
  Serial.begin(9600);
  
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  dht_indoor.begin();
}

#define SECONDS_BETWEEN_MEASUREMENTS 600
unsigned long lastTime = 0;
bool firstTime = true;

void loop() 
{  
  if ( firstTime || (millis() - lastTime > SECONDS_BETWEEN_MEASUREMENTS*1000) ) 
  {
    firstTime = false;
    lastTime = millis();

    if (!mqttClient.connected()) 
    {
      connectToWiFiAndBroker();
    }

    mqttClient.loop();

    float h_indoor = dht_indoor.readHumidity();
    float t_indoor = dht_indoor.readTemperature();

    publishFloatValue(h_indoor,"Home/GroundFloor/Humidity");
    publishFloatValue(t_indoor,"Home/GroundFloor/Temperature");

/*    int moistureWarning = digitalRead(14);
    if (moistureWarning == 1)
    {
      mqttClient.publish("Home/GroundFloor/PlantStatus", "Please water the plant!");
    }
    else
    {
      mqttClient.publish("Home/GroundFloor/PlantStatus", "Plant is ok!");
    }*/
  }

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
}

char msg[50];
void publishFloatValue(float value, char* topic)
{
    if (isnan(value)) 
    {
      Serial.println("Invalid value!");
      return;
    }

    Serial.println("Publishing a new value");
    ftoa(msg,value);
    Serial.println(msg);
    mqttClient.publish(topic, msg);
}

char *ftoa(char *buffer, float f)
{ 
  char *returnString = buffer;
  long integerPart = (long)f;
  itoa(integerPart, buffer, 10);
  while (*buffer != '\0') buffer++;
  *buffer++ = '.';
  long decimalPart = abs((long)((f - integerPart) * 100));
  itoa(decimalPart, buffer, 10);
  return returnString;
}

