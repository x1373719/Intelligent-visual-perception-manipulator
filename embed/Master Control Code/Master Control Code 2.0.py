import sys
sys.path.append("/home/loongson/.local/lib/python3.7/site-packages")

from loongpio import Servo
from loongpio import LED
from loongpio.pins import *
import serial
import time
import cv2
import numpy as nmp
import cv2.aruco as aruco

#action catch

ser=serial.Serial('/dev/ttyUSB1',115200,timeout=0.5)
time.sleep(3)
serGHT=serial.Serial('/dev/ttyACM1',115200,timeout=0.5)
time.sleep(3)

servo = Servo(PWM0)
speck1 = LED(1)
speck2 = LED(4)
speck3 = LED(5)
speck4 = LED(6)
speck5 = LED(10)
water1 = LED(7)

speck1.on()
speck2.on()
speck3.on()
speck4.on()
speck5.on()
water1.on()

speck1.off()
time.sleep(0.2)
speck1.on()
time.sleep(6)

cap=cv2.VideoCapture(0)
cap.set(3,360)
cap.set(4,240)
arcuoMtime = 0

while (arcuoMtime <500):
         retqq,frame = cap.read()
         gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
         aruco_dict = aruco.Dictionary_get(aruco.DICT_4X4_250)
         parameters = aruco.DetectorParameters_create()
         corners, ids, rejectedImgPoints = aruco.detectMarkers(gray, aruco_dict, parameters=parameters)
         aruco.drawDetectedMarkers(frame, corners,ids)
         cv2.imshow("frame", frame)
         cv2.waitKey(1)
         arcuoMtime = arcuoMtime +1

speck2.off()
time.sleep(0.2)
speck2.on()
time.sleep(6)


servo.value = 0.4
time.sleep(1.4)

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

ser.write('{#000P1500T1000!}'.encode('utf-8'))
time.sleep(1.4)


###############################################################face
zzzzz=1500
xx=0
yy=60
zz=230

xz0=1500

sersend_switch=0

face_cascade = cv2.CascadeClassifier("/home/loongson/Desktop/pythoncode/haarcascade_frontalface_default.xml")
mouth_cascade = cv2.CascadeClassifier("/home/loongson/Desktop/pythoncode/haarcascade_mcs_mouth.xml")
time.sleep(1)
ser.write('$KMS:0,60,230,1000!'.encode('utf-8'))



speck3.off()
time.sleep(0.2)
speck3.on()
time.sleep(2)








timesss=0

breakcode = 0
while (breakcode == 0):
   res,img=cap.read()
   
   if res:
      #cv2.namedWindow('1',0)
      #cv2.resizeWindow('1',512,512)
      #cv2.imshow('1',img)
      #img=nmp.rot90(img)
      #img=nmp.rot90(img)
      gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

      # 探测图片中的人脸

      faces = face_cascade.detectMultiScale(

      gray,

      scaleFactor = 1.3,

      minNeighbors = 5,

      minSize = (50,50),

      flags = cv2.CASCADE_SCALE_IMAGE

      )

      print(faces)
      for(x,y,w,h) in faces:
         
          #str1="{#000P"
          #if (x+w/2)<80:
          #     zzzzz=zzzzz+15
          #if (x+w/2)>280:
          #     zzzzz=zzzzz-15
          #if (x+w/2)<150:
          #     zzzzz=zzzzz+3
          #if (x+w/2)>210:
          #     zzzzz=zzzzz-3
          #str2="T1000!}"
          
          #sendser=str1+str(zzzzz)+str2
          if timesss<150 :
                strn1 = "$KMS:"
                strn2 = ","
                strn3 = "100!"
                if (sersend_switch == 0):
                         if (y+h/2)<110:
                            zz=zz+int(abs((y+h/2)-120)/6)
                            if(zz>250):
                               zz=250
                         if (y+h/2)>130:
                            zz=zz-int(abs((y+h/2)-120)/6)
                            if(zz<50):
                               zz=50


                if (sersend_switch == 1):      
                         if (x+w/2)<168:
                            xz0=xz0+int(abs((x+w/2)-180)/4)
                         if (x+w/2)>192:
                            xz0=xz0-int(abs((x+w/2)-180)/4)
                   
                sendsern=strn1+str(xx)+strn2+str(yy)+strn2+str(zz)+strn2+strn3
                sendsern_dibu="{#000P"+str(xz0)+"T0400!}"

                if ( int(abs((y+h/2)-120))>10   or  int(abs((x+w/2)-180))>12 ):
                         if (sersend_switch == 0):
                                 ser.write(sendsern.encode('utf-8'))
                         print(sendsern_dibu)
                         if (sersend_switch == 1):
                                 ser.write(sendsern_dibu.encode('utf-8'))
                         if (sersend_switch == 0):
                                 sersend_switch = 1
                         else :
                                 sersend_switch = 0

                


          timesss=timesss+1

          print(timesss)

          if timesss>150 :
                speck4.off()
                time.sleep(0.2)
                speck4.on()
                time.sleep(4)
                strn1 = "$KMS:"
                strn2 = ","
                strn3 = "400!"
                sendsern=strn1+str(xx)+strn2+str(yy)+strn2+str(zz)+strn2+strn3
                ser.write(sendsern.encode('utf-8'))
                time.sleep(0.5)
                yy=yy+30
                #zz=zz+5
                sendsern=strn1+str(xx)+strn2+str(yy)+strn2+str(zz)+strn2+strn3
                ser.write(sendsern.encode('utf-8'))          
                time.sleep(0.5)
                yy=yy+20
                #zz=zz+5
                sendsern=strn1+str(xx)+strn2+str(yy)+strn2+str(zz)+strn2+strn3
                ser.write(sendsern.encode('utf-8'))          
                time.sleep(0.5)
                yy=yy+20
                #zz=zz+5
                sendsern=strn1+str(xx)+strn2+str(yy)+strn2+str(zz)+strn2+strn3
                ser.write(sendsern.encode('utf-8'))          
                time.sleep(0.5)
                yy=yy+10
                #zz=zz+5
                sendsern=strn1+str(xx)+strn2+str(yy)+strn2+str(zz)+strn2+strn3
                ser.write(sendsern.encode('utf-8'))          
                time.sleep(0.5)
                yy=yy+10
                #zz=zz+5
                sendsern=strn1+str(xx)+strn2+str(yy)+strn2+str(zz)+strn2+strn3
                ser.write(sendsern.encode('utf-8'))          
                time.sleep(0.5)
                yy=yy+10

                sendsern=strn1+str(xx)+strn2+str(yy)+strn2+str(zz)+strn2+strn3
                ser.write(sendsern.encode('utf-8'))          
                time.sleep(0.5)
                yy=yy+5

                sendsern=strn1+str(xx)+strn2+str(yy)+strn2+str(zz)+strn2+strn3
                ser.write(sendsern.encode('utf-8'))          
                time.sleep(2)

                ser.write('{#004P1000T0800!}'.encode('utf-8'))
                time.sleep(1.6)

                ser.write('{#004P2000T1200!}'.encode('utf-8'))
                time.sleep(1.6)

                ser.write('{#004P1500T1200!}'.encode('utf-8'))
                time.sleep(1.6)

                ser.write('$KMS:0,100,220,1000!'.encode('utf-8'))
                time.sleep(2)
                
                ser.write('{#000P0880T1000!}'.encode('utf-8'))
                time.sleep(1.4)

                ser.write('$KMS:0,180,180,1000!'.encode('utf-8'))
                time.sleep(1.4)
                
                servo.value = 0.5
                time.sleep(2)
#######################
                
                water1.off()
                
                ser.write('{#000P2200T1000!}'.encode('utf-8'))
                time.sleep(1.4)

                ser.write('{#004P1000T0800!}'.encode('utf-8'))
                time.sleep(1.6)

                ser.write('{#004P2000T1200!}'.encode('utf-8'))
                time.sleep(1.6)

                ser.write('{#004P1500T1200!}'.encode('utf-8'))
                time.sleep(1.6)


                water1.on()
                

                breakcode = 1
          #ser.write(sendser.encode('utf-8'))
          
          cv2.rectangle(img,(x,y),(x+w,y+h),(0,255,0),2)
          imgcut=img[y+((h*5)//8):y+h,x:x+w]
          #grayface = cv2.cvtColor(imgcut,cv2.COLOR_BGR2GRAY)
          mouths = mouth_cascade.detectMultiScale(

          imgcut,

          scaleFactor = 1.1,

          minNeighbors = 5,

          minSize = (3,3),

          flags = cv2.CASCADE_SCALE_IMAGE

          )

          for(x1,y1,w1,h1) in mouths:
             
             cv2.rectangle(img,(x+x1,y+((h*5)//8)+y1),(x+x1+w1,y+((h*5)//8)+y1+h1),(0,0,255),2)

             
          
          
      cv2.namedWindow('1',0)
      cv2.resizeWindow('1',512,512)
      cv2.imshow('1',img)
      cv2.waitKey(3)




#go back
speck5.off()
time.sleep(0.2)
speck5.on()
time.sleep(6)
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
