import serial
import time

ser=serial.Serial('/dev/ttyUSB0',115200,timeout=0.5)
time.sleep(3)



ser.write('{#005P1500T1000!}'.encode('utf-8'))
time.sleep(2)

ser.write('$KMS:80,80,18,1000!'.encode('utf-8'))
time.sleep(2)

ser.write('{#005P2000T1000!}'.encode('utf-8'))
time.sleep(2)

ser.write('$KMS:0,100,150,1000!'.encode('utf-8'))
time.sleep(2)

ser.write('$KMS:-100,80,50,1000!'.encode('utf-8'))
time.sleep(2)

ser.write('{#005P1500T1000!}'.encode('utf-8'))
time.sleep(2)

ser.write('$KMS:50,100,150,1000!'.encode('utf-8'))
time.sleep(2)



