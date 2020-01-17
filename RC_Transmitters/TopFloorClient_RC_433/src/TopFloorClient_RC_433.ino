//
// An Arduino sketch for an IoT node (Arduino Mini) that sends sensor values via 433 MHz radio
// The Sensor433/RCSwitch library is used for the transmissions
// The Narcopleptic library is used for power save during delay
// Sensor values are fetched from an BPM180/085 sensor via i2C
//

#include <Wire.h>
#include <Adafruit_BMP085.h>
#include <Sensor433.h>
#include <Narcoleptic.h>

//
// Sensor setup
// The BMP085 module measures air pressure and temperature and operates via i2C
//
Adafruit_BMP085 bmp; // pin 4, SDA (data), pin 5, SDC (clock)

//
// Data transmission setup
//
#define TX_PIN 12                     // PWM output pin to use for transmission
#define TOPFLOOR_TEMP_ID    1
#define BMP_PRESSURE_ID     2
Sensor433::Transmitter transmitter = Sensor433::Transmitter(TX_PIN);

void setup()
{
  Serial.begin(9600);

  bmp.begin();
}

void loop()
{
  float temp = bmp.readTemperature();

  Serial.print("Temperature = ");
  Serial.print(temp);
  Serial.println(" *C");
  transmitter.sendFloat(TOPFLOOR_TEMP_ID, temp);

  float pressure = bmp.readPressure();
  Serial.print("Pressure (float) = ");
  Serial.print(pressure);
  Serial.println(" Pa");

  // Convert to hPha and use an offset pressure
  // of 900hPa so that the value will
  // fit as a float value in the Sensor433 protocol
  float offsetPressure = pressure/100.0 - 900.0;
  Serial.print("Pressure (offset) = ");
  Serial.print(offsetPressure);
  Serial.println(" hPa");
  transmitter.sendFloat(BMP_PRESSURE_ID, offsetPressure);

  for (int i=0; i< 100; i++)
  {
    // Max narcoleptic delay is 8s
    Narcoleptic.delay(8000);
  }
}
