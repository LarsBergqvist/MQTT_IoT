#include <Arduino.h>

//
// An Arduino sketch for an IoT node that sends sensor values via 433 MHz radio
// The Sensor433/RCSwitch library is used for the transmissions
// The Narcopleptic library is used for power save during delay
// Sensor values are fetched from two DallasTemp sensors on a common OneWire bus
// 

#include <OneWire.h>
#include <DallasTemperature.h>
#include <Sensor433.h>
#include <Narcoleptic.h>

//
// Data transmission setup
//
#define GARDEN_TEMP_ID 4
#define SOIL_TEMP_ID   5
#define TX_PIN 5
Sensor433::Transmitter transmitter = Sensor433::Transmitter(TX_PIN);

//
// OneWire / DallasTemp sensor setup
//
#define ONE_WIRE_BUS 2
#define TEMPERATURE_PRECISION 9
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);
DeviceAddress soilThermometer, outsideThermometer;

void setup(void)
{
  Serial.begin(9600);

  sensors.begin();

  Serial.print("Found ");
  Serial.print(sensors.getDeviceCount(), DEC);
  Serial.println(" devices.");

  if (!sensors.getAddress(soilThermometer, 0)) Serial.println("Unable to find address for Device 0"); 
  if (!sensors.getAddress(outsideThermometer, 1)) Serial.println("Unable to find address for Device 1"); 

  Serial.println();

  // set the resolution to 9 bit
  sensors.setResolution(soilThermometer, TEMPERATURE_PRECISION);
  sensors.setResolution(outsideThermometer, TEMPERATURE_PRECISION);
}

void loop(void)
{ 
  sensors.requestTemperatures();

  float gardenTemp = sensors.getTempC(outsideThermometer);
  float soilTemp = sensors.getTempC(soilThermometer);
  Serial.print("Outside temp:");
  Serial.println(gardenTemp);
  Serial.print("Soil temp:");
  Serial.println(soilTemp);

  transmitter.sendFloat(GARDEN_TEMP_ID,gardenTemp);
  transmitter.sendFloat(SOIL_TEMP_ID,soilTemp);

  for (int i=0; i< 100; i++)
  {
    // Max narcoleptic delay is 8s
    Narcoleptic.delay(8000);
  }
}


