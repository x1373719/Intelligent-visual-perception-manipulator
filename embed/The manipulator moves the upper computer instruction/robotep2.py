import serial
import time

ser=serial.Serial('/dev/ttyUSB0',115200,timeout=0.5)
time.sleep(3)



ser.write('{#005P1000T1000!}'.encode('utf-8'))
time.sleep(2)

ser.write('$KMS:0,100,200,1000!'.encode('utf-8'))
time.sleep(2)

ser.write('$KMS:-200,0,100,1000!'.encode('utf-8'))
time.sleep(2)

ser.write('$KMS:-200,0,17,1000!'.encode('utf-8'))
time.sleep(2)


ser.write('{#005P1900T1000!}'.encode('utf-8'))
time.sleep(2)



ser.write('$KMS:-200,0,150,1000!'.encode('utf-8'))
time.sleep(2)

ser.write('$KMS:0,100,200,1000!'.encode('utf-8'))
time.sleep(2)


ser.write('$KMS:0,230,170,1000!'.encode('utf-8'))
time.sleep(2)


ser.write('$KMS:0,240,140,1000!'.encode('utf-8'))
time.sleep(2)

ser.write('{#005P1000T1000!}'.encode('utf-8'))
time.sleep(2)


ser.write('$KMS:0,100,200,1000!'.encode('utf-8'))
time.sleep(2)
