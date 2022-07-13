import sys
sys.path.append("/home/loongson/.local/lib/python3.7/site-packages")

from loongpio import LED
from time import sleep

led = LED(4)

while True :
        led.on()
        sleep(3)
        led.off()
        sleep(3)
