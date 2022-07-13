import cv2
import numpy as nmp
import sys


face_cascade = cv2.CascadeClassifier(r'./cascade.xml')#cv2.CascadeClassifier(r'./haarcascade_frontalface_default.xml')
cap=cv2.VideoCapture(0)
#('./380pt.mp4')
cap.set(3,360)
cap.set(4,240)
while True:
   res,img=cap.read()
   
   if res:
      #cv2.namedWindow('1',0)
      #cv2.resizeWindow('1',512,512)
      #cv2.imshow('1',img)
      gray = cv2.cvtColor(img,cv2.COLOR_BGR2GRAY)

      # 探测图片中的人脸

      faces = face_cascade.detectMultiScale(

      gray,

      scaleFactor = 1.1,

      minNeighbors = 5,

      minSize = (5,5),

      flags = cv2.CASCADE_SCALE_IMAGE

      )


      #print(faces)
      #x1=faces[1]
      #y1=faces[1]
      print(faces)
      #faceimg=
      for(x,y,w,h) in faces:

          cv2.rectangle(img,(x,y),(x+w,y+h),(0,255,0),2)
          imgcut=img[y:y+h,x:x+w]
          cv2.imshow('2',imgcut)

          
      cv2.namedWindow('1',0)
      cv2.resizeWindow('1',512,512)
      cv2.imshow('1',img)
      
   if cv2.waitKey(1) & 0xFF == ord('q'):
       break
   #cv2.waitKey(20)


   
cv2.namedWindow('12',0)
cap.release()

   

