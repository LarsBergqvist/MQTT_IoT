#!/usr/bin/env python3
import paho.mqtt.publish as publish
import sys
import speedtest

def get_download_speed():
    s=speedtest.Speedtest()
    s.get_best_server()
    return str(int(s.download()/1000000))

def publish_message(topic, message):
    print("Publishing to MQTT topic: " + topic)
    print("Message: " + message)

    publish.single(topic, message, hostname="192.168.1.16")

if __name__ == '__main__':
    if len(sys.argv) != 2:
        print("Specify a computer name as argument to " + __file__)
        sys.exit(2)

    computer_name = sys.argv[1]
    print("Doing measurements for: " + computer_name)

    publish_message("Home/" + computer_name + "/DownloadSpeed", get_download_speed())
