import sys
sys.path.append("/home/loongson/.local/lib/python3.7/site-packages")

from loongpio import Servo
from loongpio.pins import *
import serial
import time

ser=serial.Serial('/dev/ttyUSB0',115200,timeout=0.5)
time.sleep(3)

servo = Servo(PWM0)

servo.value = 0.4
time.sleep(1)

ser.write('{#000P0800T1000!}'.encode('utf-8'))
time.sleep(1.4)

ser.write('$KMS:0,60,230,1000!'.encode('utf-8'))
time.sleep(1.4)

ser.write('$KMS:0,280,230,1000!'.encode('utf-8'))
time.sleep(1.4)

servo.value = 1.25
time.sleep(1.3)

ser.write('$KMS:0,60,230,1000!'.encode('utf-8'))
time.sleep(1.4)











#go back

ser.write('{#000P1500T1000!}'.encode('utf-8'))
time.sleep(2)
ser.write('{#001P1500T1000!}'.encode('utf-8'))
time.sleep(2)
ser.write('{#002P1500T1000!}'.encode('utf-8'))
time.sleep(2)
ser.write('{#003P1500T1000!}'.encode('utf-8'))
time.sleep(2)
ser.write('{#004P1500T1000!}'.encode('utf-8'))
time.sleep(2)
