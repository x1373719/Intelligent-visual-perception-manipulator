import serial
import time
ser=serial.Serial('COM6',115200,timeout=0.5)
time.sleep(3)
ser.write('AT+CMGF=1\r\n'.encode('utf-8'))
time.sleep(1.1)
ser.write('AT+CSMP=17,167,0,0\r\n'.encode('utf-8'))

time.sleep(1.1)
ser.write('AT+CMGS="13296588098"\r\n'.encode('utf-8'))

time.sleep(1.1)
ser.write('hello,2091,NICE'.encode('utf-8'))

time.sleep(1.1)

ser.write(b'\x1A')

time.sleep(1.1)

print("OK")


