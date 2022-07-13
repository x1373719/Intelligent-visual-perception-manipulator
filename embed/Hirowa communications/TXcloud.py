import serial
import time
ser=serial.Serial('COM6',115200,timeout=0.5)
time.sleep(3)

ser.write('AT+MIPCALL=1\r\n'.encode('utf-8'))
time.sleep(0.3)

ser.write('AT+MIPCALL?\r\n'.encode('utf-8'))
time.sleep(0.3)

ser.write('AT+TCDEVINFOSET=1,"EES7ENOZFA","weilaiRoad_Lamp001","AZGW8Krjwb9WfBXMwqX68A=="\r\n'.encode('utf-8'))
time.sleep(0.3)

ser.write('AT+TCMQTTCONN=1,20000,240,1,1\r\n'.encode('utf-8'))
time.sleep(0.3)

ser.write('AT+TCMQTTSUB="$thing/down/property/EES7ENOZFA/weilaiRoad_Lamp001",1\r\n'.encode('utf-8'))
time.sleep(0.3)

ser.write('AT+TCMQTTPUB="$thing/up/property/EES7ENOZFA/weilaiRoad_Lamp001",1,"{\\"method\\":\\"report\\",\\"clientToken\\":\\"123\\",\\"params\\":{\\"power_switch1\\":1}}"\r\n'.encode('utf-8'))
time.sleep(0.3)

print('AT+TCMQTTPUB="$thing/up/property/EES7ENOZFA/weilaiRoad_Lamp001",1,"{\\"method\\":\\"report\\",\\"clientToken\\":\\"123\\",\\"params\\":{\\"power_switch\\":0}}"\r\n')

ser.write('AT+TCMQTTDISCONN\r\n'.encode('utf-8'))
time.sleep(1.1)

ser.write('AT+MIPCALL=0\r\n'.encode('utf-8'))
time.sleep(1.1)



#AT+TCMQTTPUB="$thing/up/property/EES7ENOZFA/weilaiRoad_Lamp001",1,"{\"method\":\"report\",\"clientToken\":\"123\",\"params\":{\"power_switch\":0}}"
#AT+TCMQTTPUB="$thing/up/property/EES7ENOZFA/weilaiRoad_Lamp001",1,"{\"method\":\"report\",\"clientToken\":\"123\",\"params\":{\"power_switch\":1}}"
