#!/usr/bin/env python

import sys

import serial

baudRate = 14400
neutralBaudRate = 9600
portName = "/dev/ttyACM0"

if len(sys.argv) > 1:
    portName = int(sys.argv[1])

if len(sys.argv) > 2:
    baudRate = sys.argv[2]

try:
    ser = serial.Serial(portName, baudRate)
    ser.close()
    ser = serial.Serial(portName, neutralBaudRate)
    ser.close()

except Exception:
    pass
