import numpy as np
import time
import cv2
import cv2.aruco as aruco

cap = cv2.VideoCapture(0)

while(1):
        # 读取图片
        #frame = cv2.imread('E[$T%%{T9KY{$_VG)@QT`44.png')
        # frame=cv2.imread('2.png')

        retqq,frame = cap.read()
        
        # 调整图片大小
        frame=cv2.resize(frame, None, fx=0.3, fy=0.3, interpolation=cv2.INTER_CUBIC)
        # 灰度话
        gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
        # 设置预定义的字典
        aruco_dict = aruco.Dictionary_get(aruco.DICT_4X4_250)
        # 使用默认值初始化检测器参数
        parameters = aruco.DetectorParameters_create()
        # 使用aruco.detectMarkers()函数可以检测到marker，返回ID和标志板的4个角点坐标
        corners, ids, rejectedImgPoints = aruco.detectMarkers(gray, aruco_dict, parameters=parameters)



        corners1=(          np.array([[[200., 731.],[628., 735.],[608., 871.],[412., 868.]]],dtype="float32")      ,         )
        #这是一个奇特的元组，第一个元素是np.array（数据类型是float32）,第二个元素是空
        #corners1=(          np.array([[[404., 731.],[628., 735.],[608., 871.],[412., 868.]]],dtype="float32")      ,         )
        aaa=[[1,2],[2,3]]
        ids11 = np.array([[2],[3]])
        ids111222 = ids11.tolist()
        print(aaa)
        print(type(aaa))
        print(ids11)
        print(corners)
        print(corners1)
        # 画出标志位置
        aruco.drawDetectedMarkers(frame, corners,ids)


        cv2.imshow("frame", frame)
        cv2.waitKey(1)
#cv2.destroyAllWindows()

