#!/usr/bin/env python
# from __future__ import print_function
import roslib

roslib.load_manifest('team105')
import rospy
import cv2
from std_msgs.msg import Bool
from sensor_msgs.msg import CompressedImage, Image
from cv_bridge import CvBridge, CvBridgeError
import numpy as np
import time

from param import Param
from car_control import CarControl
#from sign_detection import SignDetection
from depth_camera import DepthCamera
#from angle_detection import predict as predict_angle
from lcd_control import LCDControl

from localization import CarPose, Position, CheckPoint
import tf
import geometry_msgs.msg

class ImageConverter:
    def __init__(self):
        self.is_go = True
        self.go_sub = rospy.Subscriber("/ss_status", Bool, callback=self.callback_ss, queue_size=1)

        self.param = Param()
        self.cc = CarControl(self.param)
        #self.sd = SignDetection(self.param)
        self.dc = DepthCamera()
        self.lcd = LCDControl(self.param)

        self.sd_counter = 0
        self.cc_counter = 0
        self.dc_counter = 0
        self.bt_counter = 0
        self.st_counter = 0

        self.sign = 0
        self.bbox_obstacles = []
        self.danger_zone = (0, 0)

        self.curr_time = "100"

        # 1->2->3
        list_state = [{"start": '1',  # 1->2
                    '1': 's',
                    '2': 'r',
                    '3': 's'},
                    {"start": '2', # 2->3
                    '1': 's',
                    '2': 'r',
                    '3': 'l'},
                    {"start": '3',  # 2->3
                    '1': 's',
                    '2': 's',
                    '3': 'l'}]
                    
        self.listener = tf.TransformListener()
        self.car_pose = CarPose(self.param.checkpoint1, self.param)
        self.car_pose.update_list_state(list_state)
        
        self.rate = rospy.Rate(10)

        self.is_turning = False

        self.bridge = CvBridge()
        self.image_cc_sub = rospy.Subscriber("camera/rgb/image_raw/compressed", CompressedImage, callback=self.callback,queue_size=1)
        #self.image_sd_sub = rospy.Subscriber("camera/rgb/image_raw/compressed", CompressedImage, callback=self.callback_detect_sign, queue_size=1)

        #self.depth_sub = rospy.Subscriber("/camera/depth/image_raw", Image, callback=self.callback_detect_obstacle, queue_size=1)

        #self.angle_sub = rospy.Subscriber("camera/rgb/image_raw/compressed", CompressedImage, callback=self.callback_detect_angle, queue_size=1)

        self.fps_counter_test = 0
        self.fps_timer = time.time()

        self.slow_down = 0
        self.right_angle = 0
        self.sign_distance = False

    def callback_ss(self, data):
        self.is_go = data.data

    def callback(self, data):
        try:
            self.cc_counter += 1
            if self.cc_counter % 2 == 0:
                self.cc_counter = 0
                try: 
                    if self.is_go:
                        # Localization
                        # if self.listener.waitForTransform("/camera_link", "/camera_pose", rospy.Time(0), rospy.Duration(0)):
                    	(trans, rot) = self.listener.lookupTransform('/camera_link', '/camera_pose', rospy.Time(0))

                    	x, y, z = trans[0] * 1000, trans[1] * 1000, trans[2] * 1000
                    	self.update_pos(Position(x, y, z))
                except tf.LookupException:
                    pass
                np_arr = np.fromstring(data.data, np.uint8)
                image_np = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
                # image_np = self.bridge.imgmsg_to_cv2(data)

                # NOTE: image_np.shape = (240,320,3)
                image_np = cv2.resize(image_np, (320, 240))
                # drive
                #print('Right angle: {}, sign: {}'.format(self.right_angle, self.sign_distance))

                self.slow_down = self.right_angle + self.sign_distance
                self.is_turning, steer_angle, speed = self.cc.control(image_np, self.car_pose, self.sign, self.is_go, self.danger_zone, self.slow_down)

            self.bt_counter += 1
            if self.bt_counter % 10 == 0:
                self.bt_counter = 0
                self.lcd.check_button()

        except CvBridgeError as e:
            print(e)

    def callback_detect_angle(self, data):
        try:
            self.st_counter += 1
            if self.st_counter % 6 == 0:
                self.st_counter = 0

                np_arr = np.fromstring(data.data, np.uint8)
                image_np = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
                # image_np = self.bridge.imgmsg_to_cv2(data)

                # NOTE: image_np.shape = (240,320,3)
                image_np = cv2.resize(image_np, (320, 240))
                self.right_angle = predict_angle(image_np) * 2

        except CvBridgeError as e:
            print(e)

    def callback_detect_sign(self, data):
        try:
            self.sd_counter += 1
            if self.sd_counter % 5 == 0:
                self.sd_counter = 0
                np_arr = np.fromstring(data.data, np.uint8)
                image_np = cv2.imdecode(np_arr, cv2.IMREAD_COLOR)
                # image_np = self.bridge.imgmsg_to_cv2(data)

                img_out, sign_list, p_list, self.sign_distance = self.sd.detect_sign(image_np)
                cv2.imshow("sign_detection", img_out)
                cv2.waitKey(1)

                # always get the max P of list
                if len(sign_list) > 0:
                    index_max = np.argmax(np.array(p_list)[:, 1])
                    self.sign = sign_list[index_max][1]

                else:
                    self.sign = 0

        except CvBridgeError as e:
            print(e)

    def callback_detect_obstacle(self, data):

        try:
            self.dc_counter += 1
            if self.dc_counter % 2 == 0:
                self.dc_counter = 0

                self.fps_counter_test += 1
                if time.time() - self.fps_timer > 1:
                    print(self.fps_counter_test)
                    self.fps_counter_test = 0
                    self.fps_timer = time.time()

                # image_np = self.dc.process_compressedDepth(data)
                image_np = self.bridge.imgmsg_to_cv2(data)
		
                image_np = cv2.resize(image_np, (320, 240))
		
                image_np = image_np[100:, :]
		
                #cv2.imshow('img_depth', image_np)
                #cv2.waitKey(1)
                # timer = time.time()
                self.danger_zone = self.dc.pre_processing_depth_img(image_np * 10)
                # timer = time.time() - timer
                # print("time: ", timer)
                # print(time.time() - timer)

                # print(self.danger_zone)

        except CvBridgeError as e:
            print(e)

    def update_pos(self, pos):
        self.car_pose.update_position(pos)
        self.print_pos()

    def print_pos(self):

        print("car_pose", self.car_pose.position.x, self.car_pose.position.y, self.car_pose.position.z)
        print("list_state", len(self.car_pose.list_state))
        print("current_state_start", self.car_pose.current_state["start"])
        print("check_point", self.car_pose.last_checkpoint.name)
        print("turn", self.car_pose.turn)
        print("------------------")


if __name__ == '__main__':
    rospy.init_node('team105', anonymous=True)
    ic = ImageConverter()
    rospy.spin()
