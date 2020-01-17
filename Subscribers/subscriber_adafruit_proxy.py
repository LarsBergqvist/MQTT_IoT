#!/usr/bin/env python3
import paho.mqtt.client as mqtt
import datetime
import time
from Adafruit_IO import MQTTClient
from adafruit_credentials import ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY
from mylogger import logger

#
# Subscribes to messages via a local MQTT broker
# and forwards them to a cloud service (Adafruit IO)
#

def on_connected(client, userdata, flags, rc):
    print("Connected to local MQTT broker with result code "+str(rc))
    client.subscribe("Home/Outdoor/Temperature")
    client.subscribe("Home/Outdoor/Humidity")
    client.subscribe("Home/Garden/Temperature")
    client.subscribe("Home/GroundFloor/Temperature")
    client.subscribe("Home/GroundFloor/Humidity")
    client.subscribe("Home/Porch/Motion")
    client.subscribe("Home/RPI3/Temp")
    client.subscribe("Home/FrontDoor/Status")

def on_disconnected(client,userdata,rc):
    print("Disconnected from local MQTT broker")
    try_connect_to_local_broker(client)

def adafruit_connected(client):
    print("Connected to Adafruit IO")

def on_message(client, userdata, msg):
    print(str(datetime.datetime.now()) + ": " + msg.topic + " " + str(msg.payload))    
    #
    # Forward the data to Adafruit IO. Replace topic with a valid feed name
    #
    feedname=msg.topic.replace("/","_")
    print("Publish to Adafruit feedname: " + feedname)

    # Initialize the client that should connect to io.adafruit.com
    adafruitClient = MQTTClient(ADAFRUIT_IO_USERNAME, ADAFRUIT_IO_KEY,service_port=1883)
    adafruitClient.on_connect = adafruit_connected
    adafruitClient.connect()
    adafruitClient.loop()
    adafruitClient.publish(feedname,msg.payload)
    print("Finished Publish to Adafruit " + feedname)

def try_connect_to_local_broker(client):
    print("trying to connect to local broker")
    connOK=False
    while(connOK == False):
        try:
            client.connect("192.168.1.16", 1883, 60)
            connOK = True
        except:
            connOK = False
        time.sleep(2)

#
# Initialize the client that should connect to the local MQTT broker
#
client = mqtt.Client()
client.on_connect = on_connected
client.on_disconnect = on_disconnected
client.on_message = on_message

while True:
    # Blocking loop to the local Mosquitto broker
    try:  
        try_connect_to_local_broker(client)
        client.loop_forever()
    except:
        print("Exception from client.loop_forever")
    time.sleep(20)
