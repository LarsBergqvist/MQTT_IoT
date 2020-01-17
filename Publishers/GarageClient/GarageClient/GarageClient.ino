#include <Arduino.h>

//
// This is an example on MQTT publish from an ESP8266 board
// to an MQTT broker (I have used a local Mosquitto running on a Raspberry Pi)
// This example uses the PubSub client library (https://github.com/knolleary/pubsubclient)
// Install it in the Arduino IDE before compiling the sketch
// Sensor values are fetched from an indoor DHT22 sensor and an outdoor DHT21 sensor


//#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WIFI_and_broker_parameters.h"

//
// WIFI and MQTT setup
//
#define CLIENT_NAME "GarageClient"
WiFiClient wifiClient;
PubSubClient mqttClient(BROKER_IP,BROKER_PORT,wifiClient);

//
// DHT Sensor setup
//
#include <Adafruit_Sensor.h>
#include <DHT.h>
#define DHTPIN_INDOOR 5
#define DHTTYPE_INDOOR DHT22
DHT dht_indoor(DHTPIN_INDOOR, DHTTYPE_INDOOR);
#define DHTPIN_OUTDOOR 4
#define DHTTYPE_OUTDOOR DHT21
DHT dht_outdoor(DHTPIN_OUTDOOR, DHTTYPE_OUTDOOR);

void setup()
{
  Serial.begin(9600);

  WiFi.begin(WLAN_SSID, WLAN_PASS);

  dht_indoor.begin();
  dht_outdoor.begin();
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
    float h_outdoor = dht_outdoor.readHumidity();
    float t_outdoor = dht_outdoor.readTemperature();
    Serial.println(h_outdoor);
    Serial.println(t_outdoor);

    publishFloatValue(h_indoor,"Home/Garage/Humidity");
    publishFloatValue(t_indoor,"Home/Garage/Temperature");
    publishFloatValue(h_outdoor,"Home/Outdoor/Humidity");
    publishFloatValue(t_outdoor,"Home/Outdoor/Temperature");
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
