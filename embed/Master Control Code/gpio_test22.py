import sys
sys.path.append("/home/loongson/.local/lib/python3.7/site-packages")

from loongpio import Servo
from loongpio.pins import *
from time import sleep

servo = Servo(PWM0)
while True :
        servo.value = 0.5
        sleep(6)
        servo.value = 1.32
        sleep(6)
