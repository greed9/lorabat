import serial
import time
import sys

ser = serial.Serial( "/dev/ttyACM0", 9600 )
ser.flushInput( )
val = ser.readline ( )
val = ser.readline ( )
print ( val.decode( "utf-8") )

