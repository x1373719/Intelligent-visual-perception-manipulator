import sys
sys.path.append("/home/loongson/.local/lib/python3.7/site-packages")

from loongpio import Servo
from loongpio.pins import *
import serial
import time

servo = Servo(PWM0)
ser=serial.Serial('/dev/ttyUSB1',115200,timeout=0.5)
time.sleep(3)

servo.value = 0.5
time.sleep(3)

servo.value = 1.3
time.sleep(3)

ser.write('{#000P0880T1000!}'.encode('utf-8'))
time.sleep(1.4)

ser.write('$KMS:0,180,180,1000!'.encode('utf-8'))
time.sleep(9)



servo.value = 0.5
time.sleep(2)




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
