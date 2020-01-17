//
// This is an example on MQTT publish from an ESP8266 board
// to an MQTT broker (I have used a local Mosquitto running on a Raspberry Pi)
// This example uses the PubSub client library (https://github.com/knolleary/pubsubclient)
// Install it in the Arduino IDE before compiling the sketch
// Sensor values are fetched from an BPM180/085 sensor via i2C
// This version of the sketch uses deep sleep to save energy while waiting to do the next measurement
// After uploading the sketch, connect a wire between pin 16 and RST to enable wake-up after deep sleep


#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "WIFI_and_broker_parameters.h"
#include <Wire.h>
#include <Adafruit_BMP085.h>

//
// WIFI and MQTT setup
//
#define CLIENT_NAME "TopFloorClient"
WiFiClient wifiClient;
PubSubClient mqttClient(BROKER_IP,BROKER_PORT,wifiClient);

//
// Sensor setup
//
Adafruit_BMP085 bmp; // pin 4, SDA (data), pin 5, SDC (clock)

#define SLEEP_SECONDS 60*15
void setup() 
{
  Serial.begin(9600);
  
  WiFi.begin(WLAN_SSID, WLAN_PASS);

  bmp.begin();

  connectToWiFiAndBroker();

  mqttClient.loop();

  float temp = bmp.readTemperature();
  
  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println(" *C");

  float pressure = bmp.readPressure();
  Serial.print("Pressure = ");
  Serial.print(pressure);
  Serial.println(" Pa");

  publishFloatValue(temp,"Home/TopFloor/Temperature");
  publishFloatValue(pressure,"Home/TopFloor/Pressure");

  ESP.deepSleep(SLEEP_SECONDS * 1000000);
}

void loop() 
{
  // Do nothing as we are the board will be reset after deep sleep  
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

