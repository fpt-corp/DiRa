#!/usr/bin/python2
import time
import rospy
import cv2
from std_msgs.msg import Float32, Bool
from sensor_msgs.msg import CompressedImage
import numpy as np
import math

#auto_control
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

class Car:
    def __init__(self, joy, hal):
        # some common variable to control the car
        self.speed = 0.0
        self.steer = 0.0
        self.recording = False
        self.has_image = False
        self.image_np = None
        self.img = None
        self.image_name = None
        self.bienbao = False

        self.subscriber = rospy.Subscriber("/camera/rgb/image_raw/compressed", CompressedImage, self.callback, queue_size=1)
        self.speed_pub = rospy.Publisher("/set_speed_car_api",Float32,queue_size=10)
        self.steerAngle_pub = rospy.Publisher("/set_steer_car_api",Float32,queue_size=10)
        self.bienBao = rospy.Subscriber("/bien_bao", Bool, self.BienBao, queue_size=1)
        #cv2.namedWindow("image_name")
        #cv2.namedWindow("color")
        #joyControl
        self.joy = joy
        #auto_control
	self.hal = hal


        self.model = Sequential()
        self.model.add(Lambda(lambda x: x/127.5 - 1.0, input_shape=(240, 320, 3)))
        self.model.add(Conv2D(24, 5, 5, activation='elu', subsample=(2, 2)))
        self.model.add(Conv2D(36, 5, 5, activation='elu', subsample=(2, 2)))
        self.model.add(Conv2D(48, 5, 5, activation='elu', subsample=(2, 2)))
        self.model.add(Conv2D(64, 3, 3, activation='elu', subsample=(2, 2)))
        self.model.add(Conv2D(64, 3, 3, activation='elu', subsample=(1, 1)))
        self.model.add(Conv2D(64, 3, 3, activation='elu', subsample=(1, 1)))
        self.model.add(Dropout(0.2))
        self.model.add(Flatten())
        self.model.add(Dense(128, activation='elu'))
        self.model.add(Dense(64, activation='elu'))
        self.model.add(Dense(32, activation='elu'))
        self.model.add(Dense(1))
        self.model.load_weights("/home/nvidia/catkin_ws/src/team515/thi/Round_2.h5")


    def BienBao(self, bienbao):
        if(bienbao.data is not None):
            if (bienbao.data == True):
                self.bienbao = True       
        

    def callback(self, rgb_data):
        np_arr = np.fromstring(rgb_data.data, np.uint8)
        self.image_np = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
        self.img = self.image_np.copy()
        self.has_image = True
        cv2.imshow("color", self.image_np)
        if(self.recording and self.joy.change == 0):
            if(self.has_image):
				self.image_name = "/home/nvidia/Data/"+str(time.time())+"_"+str(self.speed)+"_"+str(self.steer)+".jpg"
				cv2.imshow("image_name", self.img)
				cv2.imwrite(self.image_name, self.img)
            else:
                self.image_name = ""
        cv2.waitKey(1)

    def joy_control(self):
        #while(True):
		print("joy_control\t" + str(self.joy.mode) + "\t" + str(self.recording) + "\t" + str(self.speed) + "\t" + str(self.steer))
		self.recording = self.joy.recording
		self.steer = self.joy.steer
		self.speed = self.joy.speed
		self.speed_pub.publish(self.speed)
		self.steerAngle_pub.publish(self.steer + 4.0)
		if (self.joy.stop == 1):
			self.recording = False
			self.steer = 0.0
			self.speed = 0.0
			self.speed_pub.publish(self.speed)
			self.steerAngle_pub.publish(self.steer)
			print("Exit by key")
			print(str(self.joy.mode) + "\t" + str(self.recording) + "\t" + str(self.speed) + "\t" + str(self.steer))
			exit()
		time.sleep(0.01)
            

    def auto_control(self):
        #while (True):
		if(self.has_image):
			self.speed = self.hal.speed
			self.hal.pubLcdPrint()
			img = cv2.resize(self.img, (320, 240))
			self.steer = float(self.model.predict(img.reshape(1,240,320,3)))
                        #self.speed = int(speed)
			if (self.joy.pause == True or self.hal.ss_stt == False):
				self.speed = 0.0
				self.bienbao = False
			#if(self.bienbao == True and self.speed > 0.0):
				#self.speed = -100
			print ("auto_control\t" + "\t" + str(self.speed) + "\t" + str(self.steer))
			self.speed_pub.publish(self.speed)
			self.steerAngle_pub.publish(self.steer)
			if (self.joy.stop == 1):
				self.steer = 0.0
				self.speed = 0.0
				self.speed_pub.publish(self.speed)
				self.steerAngle_pub.publish(self.steer)
				print("Exit by key")
				print(str(self.speed) + "\t" + str(self.steer))
				exit()
    def control(self):
		while (True):
			if (self.joy.change == 0):
				self.joy_control()
				self.hal.speed = 0.0
			if (self.joy.change == 1):
				self.auto_control()
