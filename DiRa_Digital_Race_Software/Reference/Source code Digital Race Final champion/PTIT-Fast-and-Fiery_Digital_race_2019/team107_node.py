#!/usr/bin/env python
# import os
# os.environ['TF_CPP_MIN_LOG_LEVEL'] = '3'
import roslib

roslib.load_manifest('team107')
import sys
import rospy
import cv2
from std_msgs.msg import String, Float32, Bool
from sensor_msgs.msg import CompressedImage, Image
import numpy as np

import time
import rospkg
from steer import SegmentToSteer
from cv_bridge import CvBridge, CvBridgeError
from collections import deque
from depth_process import *

rospack = rospkg.RosPack()
path = rospack.get_path('team107') + '/scripts/'
end = time.time()
end_depth = time.time()
start = time.time()
check = True
is_running = True


# t1 = 0
class Processor:
    def __init__(self, model, sign_model):
        self.cv_bridge = CvBridge()
        self.image = None
        self.depth_image = None
        self.frame = 0
        self.flag = 0
        self.model = model
        self.sign_model = sign_model
        self.ss_sub = rospy.Subscriber('ss_status', Bool, self.run_callback, queue_size=1)
        self.image_sub = rospy.Subscriber('/camera/rgb/image_raw/compressed', CompressedImage, self.callback,
                                          queue_size=1)
        self.depth_sub = rospy.Subscriber('/camera/depth/image_raw', Image, self.depth_callback, queue_size=1)
        self.pub_speed = rospy.Publisher('/set_speed_car_api', Float32, queue_size=1)
        self.pub_steerAngle = rospy.Publisher('/set_steer_car_api', Float32, queue_size=1)
        self.lastTime = time.time()
        self.s2s = SegmentToSteer(square=3, margin=30, roi=0.4)
        self.left_restriction = 0
        self.right_restriction = 319
        self.obstacle_time = 0.0
        self.total_time = 0.0
        self.total_time_thresh = 8.0

    def depth_callback(self, data):
        global end_depth
        global is_running
        delta = time.time() - end_depth
        # if delta >= 0.03 and self.total_time < self.total_time_thresh:
        if delta >= 0.03:
            try:
                # convert_data(data.data)
                img = self.cv_bridge.imgmsg_to_cv2(data, "passthrough")
                img = cv2.resize(img, (320, 160))
                # self.left_restriction, self.right_restriction = self.check_obstacle_new(img, delta)
                self.depth_preprocess(img, 16)
                # img *= 10
                # self.depth_preprocess1(img, delta)
                # cv2.imshow('depth_left', img[90:159,  :int(0.25*IMG_W)])
                # cv2.imshow('depth', img)
                self.total_time += delta
            except CvBridgeError, e:
                print e
            end_depth = time.time()

    def depth_preprocess(self, frame, ratio):
        # time1 = time.time()
        # frame = np.float32(frame)
        # frame *= 255.0/65535
        # frame = np.uint8(frame)
        # frame = preprocess_img(frame, 32)
        # frame = remove_road(frame, road_distance=600, padding=150)
        # frame = remove_far(frame, far_distance=1200)
        # frame = remove_noise(frame, k=3)
        # rects = find_obstacles(frame)
        frame = preprocess_img(frame, ratio)
        frame = cv2.GaussianBlur(frame, (5, 5), 0)
        # print(frame)
        frame = find_object(frame, 40)
        frame = remove_noise(frame, k=5)
        # cv2.imshow('thresh', frame*255.)
        # cv2.waitKey(1)
        rects = find_obstacles(frame)
        # print rects
        self.left_restriction, self.right_restriction = get_restriction(rects)

    def depth_preprocess1(self, img, delta):
        lower_y = int(4 * 160 / 16)
        upper_y = int(12 * 160 / 16)
        img = img[lower_y:upper_y]
        img = np.float32(img)
        img *= 255.0 / 65535
        img = np.uint8(img)
        img = cv2.medianBlur(img, 5)
        # img *= 10
        ret, thresh1 = cv2.threshold(img, 3, 255, cv2.THRESH_BINARY)
        self.check_obstacle_2(thresh1, 0.5, delta)

    def check_obstacle_new(self, img, interval):
        IMG_H, IMG_W = img.shape
        lower_y = int(3*IMG_H/16)
        upper_y = int(11*IMG_H/16)
        border_x = int(0.05 * IMG_W)
        img = img[lower_y:upper_y, border_x:-border_x]
        img = np.float32(img)
        img *= 255.0/65535
        img = np.uint8(img)
        img = cv2.medianBlur(img, 5)
        left_obstacle = False
        right_obstacle = False
        middle = (IMG_W - 2 * border_x) / 2
        ratio = 0.5
        if self.obstacle_time > 0 and self.obstacle_time < 3:
            self.obstacle_time += interval
            return 0, IMG_W - 1

        self.obstacle_time = 0.0
        # img *= 10
        ret,thresh1 = cv2.threshold(img, 3, 255,cv2.THRESH_BINARY_INV)
        _, contours, _ = cv2.findContours(thresh1, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
        for contour in contours:
            if left_obstacle and right_obstacle:
                break
            if cv2.contourArea(contour) > 1500:
                x,y,w,h = cv2.boundingRect(contour)
                pos1 = middle - x
                pos2 = x + w - middle
                if pos2 <= 0:
                    left_obstacle = True
                elif pos1 >= 0:
                    right_obstacle = True
                elif pos1 < 0 and pos2 > 0:
                    if abs(pos1) > abs(pos2):
                        left_obstacle = True
                    elif abs(pos2) > abs(pos1):
                        right_obstacle = True
        if left_obstacle and not right_obstacle:
            return int(ratio * IMG_W), IMG_W - 1
        elif right_obstacle and not left_obstacle:
            return 0, int(ratio * IMG_W)
        elif right_obstacle and left_obstacle:
            self.obstacle_time += interval
        return 0, IMG_W - 1

    def check_obstacle_2(self, img, threshold, interval):
        IMG_H, IMG_W = img.shape
        self.left_restriction = 0
        self.right_restriction = IMG_W - 1
        left_obs = False
        right_obs = False
        i = int(0.5 * IMG_W)
        range = int(0.1 * IMG_W)
        lower_y = 0
        upper_y = IMG_H
        size = (upper_y - lower_y) * range
        max_value = 255
        ratio = 0.6
        left_border = 0.15
        right_border = 1 - left_border
        if self.obstacle_time > 0 and self.obstacle_time < 3:
            self.obstacle_time += interval
            return

        self.obstacle_time = 0.0
        while i - range >= 0:
            if left_obs and right_obs:
                break
            if not left_obs:
                check = img[lower_y:upper_y, i - range:i]
                left_check = np.sum(check) / (max_value * size)
                if left_check <= threshold:
                    left_obs = True
                    self.left_restriction = i
            if not right_obs:
                check = img[lower_y:upper_y, IMG_W - i: IMG_W - i + range]
                right_check = np.sum(check) / (max_value * size)
                if right_check <= threshold:
                    right_obs = True
                    self.right_restriction = IMG_W - i
            i -= range
        if self.left_restriction >= self.right_restriction:
            i += range
            while np.sum(img[lower_y:upper_y, self.left_restriction - range:self.left_restriction]) / (
                    max_value * size) <= threshold and self.left_restriction - range >= 0:
                self.left_restriction -= range
            while np.sum(img[lower_y:upper_y, self.right_restriction: self.right_restriction + range]) / (
                    max_value * size) <= threshold and self.right_restriction + range <= IMG_W:
                self.right_restriction += range
            if i - self.left_restriction > self.right_restriction - i:
                self.left_restriction = int(ratio * IMG_W)
                self.right_restriction = IMG_W - 1
            elif i - self.left_restriction < self.right_restriction - i:
                self.right_restriction = int((1 - ratio) * IMG_W)
                self.left_restriction = 0
            else:
                self.left_restriction = 0
                self.right_restriction = IMG_W - 1
        elif self.left_restriction >= left_border * IMG_W and self.right_restriction >= right_border * IMG_W:
            self.left_restriction = int(ratio * IMG_W)
            self.right_restriction = IMG_W - 1
        elif self.right_restriction <= right_border * IMG_W and self.left_restriction <= left_border * IMG_W:
            self.right_restriction = int((1 - ratio) * IMG_W)
            self.left_restriction = 0
        elif self.left_restriction >= 0.15 * IMG_W and self.right_restriction <= 0.85 * IMG_W:
            self.left_restriction = 0
            self.right_restriction = IMG_W - 1
            self.obstacle_time += interval

    def run_callback(self, data):
        global is_running
        is_running = data.data

    def callback(self, data):
        global end
        global is_running
        # global check
        # global start
        # global t1
        # if check == True:
        #   start = time.time()
        #   check = False
        delta = time.time() - end
        if delta >= 0.03:
            try:
                self.image = self.convert_data_to_image(data.data)
                if self.frame % 6 == 0:
                    self.flag, s = self.sign_model.predict(self.image)
                    self.frame = 0
                    print self.flag
                self.image = cv2.resize(self.image, (320, 160))
                res, sharp = self.model.predict(self.image)
                speed, steer, res = self.s2s.get_steer(self.image, res * 255., self.flag, sharp, self.left_restriction,
                                                       self.right_restriction)
                # cv2.imshow('black and white', res)
                # cv2.waitKey(1)
                # cv2.imshow('road', res)
                # cv2.waitKey(1)
                # if time.time() - start <= 10:
                #   speed = 100
                # print (1/(time.time()-t1))
                if is_running:
                    self.publish_data(speed, -steer)
                else:
                    self.s2s.total_time_steer = 0.0
                    self.total_time = 0.0
                    self.s2s.speed_memory = deque(iterable=np.zeros(5, dtype=np.uint8), maxlen=5)
                    self.s2s.error_proportional_ = 0.0
                    self.s2s.error_integral_ = 0.0
                    self.s2s.error_derivative_ = 0.0
                    self.publish_data(0, 0)
                # t1 = time.time()
                self.frame += 1
            except CvBridgeError as e:
                print(e)
            end = time.time()

    def convert_data_to_image(self, data):
        arr = np.fromstring(data, np.uint8)
        image = cv2.imdecode(arr, 1)
        return image

    def get_segment_image(self, image):
        res = self.model.predict(np.expand_dims(image / 255., axis=0))
        return np.argmax(res, axis=3)[0]

    def publish_data(self, speed, steerAngle):
        self.pub_speed.publish(speed)
        self.pub_steerAngle.publish(steerAngle)


def main(args):
    p = processor()
    rospy.init_node('team107')
    try:
        rospy.spin()
    except:
        print("Shutting down")


if __name__ == '__main__':
    main(sys.argv)

