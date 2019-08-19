#!/usr/bin/python2
import time
import rospy
import cv2
import math
from std_msgs.msg import Float32
from sensor_msgs.msg import Imu

class MPU:
    def __init__(self):
        self.angle = 0.0
        self.average = 271.3
        self.z = 375
        self.t = 375
        self._z = None
        self._t = None
        self.speed = 0.0
        self.arr = []
        self.mapFile = cv2.imread("/home/nvidia/map1.jpg")
        #mapFile = cv2.resize(mapFile, (500,500))
        self.mapFile = cv2.cvtColor(self.mapFile,cv2.COLOR_RGB2GRAY)

        th, self.mapFile = cv2.threshold(self.mapFile, 200, 255, cv2.THRESH_BINARY)
        width, height = self.mapFile.shape
        for w in range(width):
            for h in range(height):
                if(self.mapFile[w][h] == 255):
                    self.arr.append((h,w))
        
        self.subscriber = rospy.Subscriber("/imu_angle", Float32, self.Imu_angleCallBack, queue_size = 10)
        self.subscriber = rospy.Subscriber("/set_speed_car_api", Float32, self.SpeedCallBack, queue_size = 150)
        #print(self.arr)
    

    def SpeedCallBack(self, speed):
        if (speed.data is not None):
            self.speed = speed.data
        #print (self.speed)

    def Imu_angleCallBack(self, imu_angle):
        if (imu_angle.data is not None):
            self.angle = imu_angle.data;
        #else:
         #   print("angle Null")
        print (self.angle)
        self.Process()

    def Distance(self, diem):
        minI = None
        vicinity = 50
        minDistance = math.sqrt(math.pow((diem[0] - self.arr[0][0]), 2) + math.pow((diem[1] - self.arr[0][1]), 2))
        for i in self.arr:
            #if(abs(i[1] - vicinity) <= 50):
            Distance = math.sqrt(math.pow((diem[0] - i[0]), 2) + math.pow((diem[1] - i[1]), 2))
            if(minDistance > Distance):
                minI = i
                minDistance = Distance
        return minDistance, minI[0], minI[1]

    def TimNgaRe(self, diem):
        vicinity = 15
        count = 0
        x = diem[0]
        y = diem[1]
        dem = 0
        # AB, DC
        for i in range(x - vicinity, x + vicinity):
            if (self.mapFile[y - vicinity][i] == 255):
                count += 1
        for i in range(x - vicinity, x + vicinity):
            if (self.mapFile[y + vicinity][i] == 255):
                count += 1
        # AD, BC
        for i in range(y - vicinity, y + vicinity):
            if (self.mapFile[i][x - vicinity] == 255):
                count += 1
        for i in range(y - vicinity, y + vicinity):
            if (self.mapFile[i][x + vicinity] == 255):
                count += 1
        return count
    
    def Process(self):
        if (self.speed >= 5.0):
            ve = self.mapFile.copy()
            y = self.speed * 0.1 * math.cos((self.angle - self.average) * math.pi / 180.0)*(-1.0)
            x = self.speed * 0.1 * math.sin((self.angle - self.average) * math.pi / 180.0)*(1.0)
            self._z = self.z
            self._t = self.t
            self.z = self.z + y
            self.t = self.t + x
            diem = (self.t, self.z)
            cv2.line(ve, (int(self._t), int(self._z)), (int(self.t), int(self.z)), (255,0,255), 2)
            temp = self.Distance(diem)
            minPoint = (temp[1], temp[2])
            count = self.TimNgaRe(minPoint)
            #cv2.line(self.mapFile, (temp[1], temp[2]), (375, 375), (255,0,255), 2)
            cv2.imshow("map", ve)
            cv2.waitKey(1)
            print (self.t, self.z, count, diem)
            if(count <= 2):
                print("Di thang")
            if(count > 2):
                print("Ban sap toi nga " + str(count))
            time.sleep(0.01)

        

if __name__ == "__main__":
    rospy.init_node('MPUCallBack', anonymous=True)
    mpu = MPU()
    try:
        rospy.spin()
    except KeyboardInterrupt:
        exit
    
