#!/usr/bin/env python3
import paho.mqtt.publish as publish
from subprocess import check_output
from re import findall
import psutil
import sys
import time

def get_temp():
    temp = check_output(["vcgencmd","measure_temp"]).decode("UTF-8")
    return(findall("\d+\.\d+",temp)[0])

def get_disk_usage():
    return str(psutil.disk_usage('/').percent)

def get_memory_usage():
    return str(psutil.virtual_memory().percent)

def get_cpu_usage():
    return str(psutil.cpu_percent(interval=None))

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

    time.sleep(3)
    cpu_usage = get_cpu_usage()

    publish_message("Home/" + computer_name + "/Temp", get_temp())
    publish_message("Home/" + computer_name + "/DiskUsagePercent", get_disk_usage())
    publish_message("Home/" + computer_name + "/MemoryUsagePercent", get_memory_usage())
    publish_message("Home/" + computer_name + "/CpuUsagePercent", cpu_usage)
