#!/usr/bin/python2
import time
import rospy
import cv2
from std_msgs.msg import Float32, Bool
from sensor_msgs.msg import CompressedImage, Joy
import numpy as np
import math
import sys

"""
axex:
    0   left/right Axis stick left
    1   up/down Axis stick left
    2   left/right Axis stick right
    3   up/down Axis stick right
    4   cross key left/right
    5   cross key up/down
buttons:
    0   X
    1   A
    2   B
    3   Y
    4   LB
    5   RB
    6   LT
    7   RT
    8   BACK
    9   START
    10  stick click LEFT
    11  stick click right
"""

STEER_LIMIT = 60.0
MAX_SPEED = 18.0
MIN_SPEED = 0.0
speed_amount = 5.0

class JoyControl:
    def __init__(self):
        self.recording = False
        self.has_image = False
        self.image_np = None
        self.img = None
        self.image_name = None
        self.steer = 0.0
        self.speed = 0.0
        self.forceStop = False
        self.mode = 1
        self.stop = False
        self.forceSpeed = -100.0

        self.subscriber = rospy.Subscriber("/joy", Joy, self.JoyCallBack, queue_size = 10)
        self.subscriber = rospy.Subscriber("/camera/rgb/image_raw/compressed", CompressedImage, self.callback, queue_size=1)
        self.speed_pub = rospy.Publisher("/set_speed_car_api",Float32,queue_size=10)
        self.steerAngle_pub = rospy.Publisher("/set_steer_car_api",Float32,queue_size=10)

    def callback(self, rgb_data):
        np_arr = np.fromstring(rgb_data.data, np.uint8)
        self.image_np = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
        self.img = self.image_np.copy()
        self.has_image = True
        cv2.imshow("color", self.image_np)
        if(self.recording):
            if(self.has_image):
				self.image_name = "/home/nvidia/Data/Data/"+str(time.time())+"_"+str(self.speed)+"_"+str(self.steer)+".jpg"
				cv2.imshow("image_name", self.img)
				cv2.imwrite(self.image_name, self.img)
            else:
                self.image_name = ""
        cv2.waitKey(1)
    
    def JoyCallBack (self, ros_data):
        #steer
        self.steer = round(-(ros_data.axes[2])*STEER_LIMIT, 3)
        #exit
        self.stop = ros_data.buttons[0]
        if(self.forceStop==False):
            #record
            if(ros_data.buttons[6] == 1):
                self.recording = False
            if(ros_data.buttons[7] == 1):
                self.recording = True
            #mode switch
            if(ros_data.buttons[9] == 1):
                self.mode +=1
                if(self.mode == 2):
                    self.mode = 0
            #mode 0
            if (self.mode == 0):   
                self.speed = MAX_SPEED * ros_data.axes[1]
            #mode 1
            if (self.mode == 1):
                if(ros_data.axes[5] == -1):
                    self.speed = 0.0
                    if(self.speed < 10.0 and self.speed > 0):
                        self.speed = 0.0
                    if(self.speed < MIN_SPEED):
                        self.speed = MIN_SPEED
                if(ros_data.axes[5] == 1):
                    self.speed = 15.0
                    if(self.speed < 10.0):
                        self.speed = 10.0
                    if(self.speed > MAX_SPEED):
                        self.speed = MAX_SPEED

        #if(self.speed < 0):
         #   self.speed = 0

        if (ros_data.buttons[5] == 1):
            self.speed = 0.0
            self.recording = False
            self.forceStop = False

        
        if(ros_data.buttons[3]==1):
            self.forceStop = True
        #print(self.forceStop)
        self.speed = round(self.speed, 3)

    def tam(self):
        while(True):
            print("joy_control\t" + str(self.mode) + "\t" + str(self.forceStop)  + "\t" + str(self.recording) + "\t" + str(self.speed) + "\t" + str(self.steer))
            
            if(self.forceStop == True):
                self.speed = self.forceSpeed
            self.speed_pub.publish(self.speed)
            self.steerAngle_pub.publish(self.steer + 4.0)
            if (self.stop == 1):
                self.recording = False
                self.steer = 0.0
                self.speed = 0.0
                self.speed_pub.publish(self.speed)
                self.steerAngle_pub.publish(self.steer)
                print("Exit by key")
                print(str(self.mode) + "\t" + str(self.recording) + "\t" + str(self.speed) + "\t" + str(self.steer))
                exit()
            time.sleep(0.01)


def main(args):
	rospy.init_node('Joy_Control', anonymous=True)
	Joy_Control = JoyControl()
	Joy_Control.tam()
	try:
		rospy.spin()
	except KeyboardInterrupt:
		print("Shutting down ROS Image feature detector module")
	cv2.destroyAllWindows()

if __name__ == '__main__':
    main(sys.argv)
