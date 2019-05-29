#!/usr/bin/python2

import rospy
from std_msgs.msg import Float32
from std_msgs.msg import String
from std_msgs.msg import Bool
from sensor_msgs.msg import CompressedImage, Image
import cv2
import numpy as np
import sys
from cv_bridge import CvBridge
import time
import numpy as np
import math


class NodeHandle:
    def __init__(self):
        self.boxes = []
        self.rects = []
        self.rgb = None
        self.depth = None
        self.speed = 0.0
        self.flag = False
        self.bin = None
        self.bridge = CvBridge()
        self.rgb_sub = rospy.Subscriber("/camera/rgb/image_raw/compressed", CompressedImage, self.rgbCallBack, queue_size=1)
        self.depth_sub = rospy.Subscriber("/camera/depth/image_raw", Image, self.depthCallBack, queue_size = 1)
        self.speed_pub = rospy.Publisher("/bien_bao", Bool,queue_size=10)

    def rgbCallBack(self, rgb_data):
        np_arr = np.fromstring(rgb_data.data, np.uint8)
        image_np = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
        self.rgb = image_np
        #cv2.imshow("rgb", image_np)
        #cv2.waitKey(1)

    def depthCallBack(self, depth_data):
        depth_image = self.bridge.imgmsg_to_cv2(depth_data,"16UC1")
        depth_array = np.array(depth_image, dtype=np.float32)
        cv2.normalize(depth_array, depth_array, 0, 1, cv2.NORM_MINMAX)
        depth = (depth_array*255).astype(np.uint8)
        #depth = cv2.cvtColor(depth, cv2.COLOR_BGR2GRAY)
        self.depth = depth

    def detectBlue(self, src):
        #src = 
        hsvImg = cv2.cvtColor(src, cv2.COLOR_BGR2HSV)
        lower_blue = np.array([100,100,100])
        upper_blue = np.array([120,255,255])
        dst = cv2.inRange(hsvImg, lower_blue, upper_blue)
        self.bin = dst
        return dst

    def detect(self, binImg):
        height = np.size(binImg, 0)
        width = np.size(binImg, 1)
        img, contours, hierarchy = cv2.findContours(binImg, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)
        detected = False
        if contours is not None:
            for i in contours:
                x, y, w, h = cv2.boundingRect(i)
                rect = (x, y, w, h)
                aspect_ratio = float(w)/h
                area = cv2.contourArea(i)
                rect_area = w*h
                extent = float(area)/rect_area
                if extent > 0.001:
                    if aspect_ratio > 0.5 and aspect_ratio < 1.5:
                        detected = True
                        self.rects.append(rect)
    
        return detected

    def callBack(self):
        src = None
        while(True):
            if (self.rgb is not None and self.depth is not None):
                src = self.rgb.copy()
            self.boxes = []
            self.rects = []
            self.speed = 10.0
            maxDienTich = 0.0
            distance = 200.0
            if(src is not None):
                blue = self.detectBlue(src)
                temp = None            
                if (self.detect(blue) == True) :
                    for x, y, w, h in self.rects:
                        dienTich = math.sqrt(math.pow(w, 2) + math.pow(h, 2))
                        if(dienTich > maxDienTich):
                            maxDienTich = dienTich
                            temp = x,y,w,h
                            #cv2.rectangle(src, (temp[0], temp[1]), (temp[0]+temp[2], temp[1]+temp[3]), (0, 0, 255), 2)
                    if (temp is not None and maxDienTich > 50.0 and maxDienTich < 220):
                        cv2.rectangle(src, (temp[0], temp[1]), (temp[0]+temp[2], temp[1]+temp[3]), (0, 0, 255), 2)
                        dst = self.depth[temp[1]: (temp[1] + temp[3]), temp[0] : (temp[0]+temp[2])]
                        #cv2.imshow("dst", dst)
                        tam = 0.0
                        for i in range(dst.shape[0]):
                            for j in range(dst.shape[1]):
                                if(dst[i][j] != 0 and dst[i][j] < 150):
                                    distance += dst[i][j]
                                    tam += 1.0
                        if(tam != 0):
                            distance = distance/tam
                        if(distance < 110 and distance > 65):
                            self.flag = True
                        else:
                            self.flag = False

                #cv2.imshow("src", src)
                #cv2.waitKey(1)            
            #print(maxDienTich, self.speed, distance)
            if(self.flag):
                self.speed = -100.0
                time.sleep(1)
            self.speed_pub.publish(self.flag)

            time.sleep(0.05)

def main(args):
    rospy.init_node("bienbao", anonymous=True)
    nodeHandle = NodeHandle()
    nodeHandle.callBack()
    try:
        rospy.spin()
    except KeyboardInterrupt:
        print("exit")
    cv2.destroyAllWindows()        
if __name__ == "__main__":
    main(sys.argv)
