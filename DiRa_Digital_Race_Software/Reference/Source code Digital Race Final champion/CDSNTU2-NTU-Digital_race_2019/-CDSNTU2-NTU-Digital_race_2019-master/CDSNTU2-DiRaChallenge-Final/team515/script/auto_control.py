#!/usr/bin/python2
import time
import rospy
import cv2
from std_msgs.msg import Float32, Bool
from sensor_msgs.msg import CompressedImage
import numpy as np
import math
import sys

from hal_control import *

import tensorflow as tf

import keras
import json
from keras.models import Sequential, model_from_json, load_model, Model
from keras.layers import Dense, Conv2D, Flatten, MaxPool2D, Lambda, Dropout, Input
from keras.layers import ELU

from keras.models import Sequential, model_from_json, load_model, Model
from keras.layers import Dense, Conv2D, Flatten, MaxPool2D, Lambda, Dropout, Input
from keras.layers import ELU

##########
MIN_SPEED = 0
MAX_SPEED = 50

class AutoControl:
    def __init__(self, hal):
        # some common variable to control the car
        self.speed = 0.0
        self.steer = 0.0
        self.preSteer = 0.0
        self.image_np = None
        self.img = None
        self.bienbao = False
        self.loTrinh = None
        self.flag = True

        self.subscriber = rospy.Subscriber("/camera/rgb/image_raw/compressed", CompressedImage, self.callback, queue_size=1)
        self.speed_pub = rospy.Publisher("/set_speed_car_api",Float32,queue_size=10)
        self.steerAngle_pub = rospy.Publisher("/set_steer_car_api",Float32,queue_size=10)
        #auto_control
		self.hal = hal

		#Round 1
        self.model = Sequential()
        self.model.add(Lambda(lambda x: x/127.5-1.0, input_shape=(240, 320,3)))
        self.model.add(Conv2D(24, 5, 5, activation='elu', subsample=(2, 2)))
        self.model.add(Conv2D(36, 5, 5, activation='elu', subsample=(2, 2)))
        self.model.add(Conv2D(48, 5, 5, activation='elu', subsample=(2, 2)))
        self.model.add(Conv2D(64, 3, 3, activation='elu', subsample=(1, 1)))
        self.model.add(Conv2D(64, 3, 3, activation='elu', subsample=(1, 1)))
        self.model.add(Dropout(0.3))
        self.model.add(Flatten())
        self.model.add(Dense(128, activation='elu'))
        self.model.add(Dense(64, activation='elu'))
        self.model.add(Dense(32, activation='elu'))
        self.model.add(Dense(1))
        self.model.load_weights("/home/nvidia/catkin_ws/src/team515/thi/Round_1.h5")

    #def LoTrinh(self, data):
    #    self.loTrinh = data.data

    def callback(self, rgb_data):
        np_arr = np.fromstring(rgb_data.data, np.uint8)
        self.image_np = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
        cv2.imshow("color", self.image_np)
        cv2.waitKey(1)

    def auto_control(self):
        while(True):
            self.speed = self.hal.speed
            self.hal.pubLcdPrint()
            img = cv2.resize(self.image_np, (320, 240))
            self.steer = float(self.model.predict(img.reshape(1,240,320,3)))
            if (self.hal.ss_stt == False):
               self.speed = 0.0
               self.bienbao = False
               self.flag = True
            print ("auto_control" + "\t" + str(self.speed) + "\t" + str(self.steer))
            self.speed_pub.publish(self.speed)
            self.steerAngle_pub.publish(self.steer+4.0)
            time.sleep(0.01)

def main(args):
	rospy.init_node('Auto_Control', anonymous=True)
        hal = Hal()
	Auto_Control = AutoControl(hal)
	Auto_Control.auto_control()
    
	try:
		rospy.spin()
	except KeyboardInterrupt:
		print("Shutting down ROS Image feature detector module")
	cv2.destroyAllWindows()

if __name__ == '__main__':
    main(sys.argv)
