#include <Arduino.h>

//
// An Arduino sketch for an IoT node that sends sensor values via 433 MHz radio
// The Sensor433/RCSwitch library is used for the transmissions
// The Narcopleptic library is used for power save during delay
// Sensor values are fetched from DHT22 and DHT21 sensors
//

#include <Sensor433.h>
#include <Narcoleptic.h>

//
// Data transmission setup
//
#define GARAGE_TEMP_ID 10
#define GARAGE_HUMIDITY_ID 11
//#define OUTDOOR_TEMP_ID 12
//#define OUTDOOR_HUMIDITY_ID 13
#define TX_PIN 11
Sensor433::Transmitter transmitter = Sensor433::Transmitter(TX_PIN);

//
// DHT Sensor setup
//
#include <Adafruit_Sensor.h>
#include <DHT.h>
#define DHTPIN_INDOOR 10
#define DHTTYPE_INDOOR DHT22
DHT dht_indoor(DHTPIN_INDOOR, DHTTYPE_INDOOR);
//#define DHTPIN_OUTDOOR 9
//#define DHTTYPE_OUTDOOR DHT22
//DHT dht_outdoor(DHTPIN_OUTDOOR, DHTTYPE_OUTDOOR);

void setup(void)
{
  dht_indoor.begin();
//  dht_outdoor.begin();
}

void loop(void)
{
  delay(2000);

  float garageTemp = dht_indoor.readTemperature();
  float garageHumidity = dht_indoor.readHumidity();
//  float outdoorTemp = dht_outdoor.readTemperature();
//  float outdoorHumidity = dht_outdoor.readHumidity();

  if (!isnan(garageTemp))
    transmitter.sendFloat(GARAGE_TEMP_ID,garageTemp);
  if (!isnan(garageHumidity))
    transmitter.sendFloat(GARAGE_HUMIDITY_ID,garageHumidity);
  /*if (!isnan(outdoorTemp))
    transmitter.sendFloat(OUTDOOR_TEMP_ID,outdoorTemp);
  if (!isnan(outdoorHumidity))
    transmitter.sendFloat(OUTDOOR_HUMIDITY_ID,outdoorHumidity);
*/
  for (int i=0; i< 100; i++)
  {
    // Max narcoleptic delay is 8s
    Narcoleptic.delay(8000);
  }
}
