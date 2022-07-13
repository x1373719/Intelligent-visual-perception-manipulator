import serial
import time
ser=serial.Serial('/dev/ttyUSB0',115200,timeout=0.5)
time.sleep(3)
ser.write('{#000P1000T1000!#001P1300T1000!#002P1800T1000!}'.encode('utf-8'))

time.sleep(1.1)
ser.write('{#000P1500T1000!#001P1300T1000!#002P1800T1000!}'.encode('utf-8'))

time.sleep(1.1)
ser.write('{#000P1500T1000!#001P1500T1000!#002P1800T1000!}'.encode('utf-8'))

time.sleep(1.1)
ser.write('{#000P1500T1000!#001P1500T1000!#002P1500T1000!}'.encode('utf-8'))

time.sleep(1.1)
ser.write('{#000P1500T1000!#001P1800T1000!#002P1200T1000!#003P1700T1000!}'.encode('utf-8'))


time.sleep(1.1)
ser.write('{#001P1200T2000!#002P1200T2000!#003P2300T2000!}'.encode('utf-8'))
