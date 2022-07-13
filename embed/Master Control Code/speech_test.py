import sys
sys.path.append("/home/loongson/.local/lib/python3.7/site-packages")

from loongpio import LED
from loongpio.pins import *
import time

speck1 = LED(1)
speck2 = LED(4)
speck3 = LED(5)
speck4 = LED(6)
speck5 = LED(10)

speck1.on()
speck2.on()
speck3.on()
speck4.on()
speck5.on()

while True :
   speck1.off()
   time.sleep(0.2)
   speck1.on()
   time.sleep(6)
   
   speck2.off()
   time.sleep(0.2)
   speck2.on()
   time.sleep(6)
   
   speck3.off()
   time.sleep(0.2)
   speck3.on()
   time.sleep(6)

   speck4.off()
   time.sleep(0.2)
   speck4.on()
   time.sleep(6)

   speck5.off()
   time.sleep(0.2)
   speck5.on()
   time.sleep(6)


