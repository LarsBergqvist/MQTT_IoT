#!/usr/bin/env python3
import paho.mqtt.client as mqtt
import datetime
import time
import json
from influxdb import InfluxDBClient

def on_connect(client, userdata, flags, rc):
    print("Connected with result code "+str(rc))
    client.subscribe("owntracks/#")

def on_message(client, userdata, msg):
    message=msg.payload.decode("utf-8")
    try:
        print(message)
        j = json.loads(message)
        tst = j["tst"]
        lon = j["lon"]
        lat = j["lat"]
        tid = j["tid"]
        print(tst)
        print(lon)
        print(lat)
        print(tid)

        json_body = [
            {
                "measurement": tid + "_pos",
#                "time": tst,
                "fields": {
                    "value": lon,
                    "lat": lat
                }
            }
        ]

        dbclient.write_points(json_body)
        print("Stored ok!")

    except:
        print("Could not parse and save message")
        isfloatValue=False


# Set up client for InfluxDB
dbclient = InfluxDBClient('192.168.1.16', 8086, 'root', 'root', 'sensordata')

# Initialize the client that should connect to the Mosquitto broker
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
connOK=False
while(connOK == False):
    try:
        client.connect("192.168.1.16", 1883, 60)
        connOK = True
    except:
        connOK = False
    time.sleep(2)

# Blocking loop to the Mosquitto broker
client.loop_forever()
