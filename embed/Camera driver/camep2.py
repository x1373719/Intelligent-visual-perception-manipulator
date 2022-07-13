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
   cv2.namedWindow('1',0)
   cv2.resizeWindow('1',512,512)
   cv2.imshow('1',img)
   cv2.waitKey(20)
      



   

