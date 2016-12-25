#!/usr/bin/env python3
# -*- coding: utf-8 -*-



import serial
import time

def ser_recv(dev):
    while True:
        data = dev.readline()
        if data:
            return data
        time.sleep(1)

def ser_write(dev, data):
    dev.write(data)

if __name__ == "__main__":
    gps = serial.Serial("/dev/ttyAMA0", 9600, timeout=1)
    bluetooth = serial.Serial("/dev/ttyUSB0", 38400, timeout=1)
    while True:
        try:
            # recv gps data
            data = ser_recv(gps)
            print(data)
            # write to bluetooth
            ser_write(bluetooth, data)
        except:
            print("\nexit")
            break
    gps.close()
    bluetooth.close()
