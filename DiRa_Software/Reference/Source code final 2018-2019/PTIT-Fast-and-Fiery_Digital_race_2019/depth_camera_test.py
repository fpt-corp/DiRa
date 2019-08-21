#!/usr/bin/env python
import rospy
import numpy as np
import cv2
import time
import rospkg
from sensor_msgs.msg import Image, CompressedImage
from cv_bridge import CvBridge, CvBridgeError
from tensorflow.keras.models import model_from_json, load_model
from tensorflow.keras.utils import CustomObjectScope
from tensorflow.keras.initializers import glorot_uniform
from steer import SegmentToSteer
from depth_process import *

list_image = []
bridge = CvBridge()
rospack = rospkg.RosPack()
path = rospack.get_path('team107') + '/scripts/'
IMG_H = 160
IMG_W = 320
image = np.zeros((160, 320), np.float32)
color_image = np.zeros((160, 320), np.float32)
depth_image = None
obstacle_time = 0.0
i_left, i_right = 0, 319
end = time.time()
# f = open(path + 'model_mobilenet_lane.json', 'r')
# with CustomObjectScope({'GlorotUniform': glorot_uniform()}):
#     model = model_from_json(f.read())
#     model.load_weights(path + 'model-mobilenet-iter1234add-pretrain-bdd.h5')
#     model._make_predict_function()
#     print "Model loaded"
s2s = SegmentToSteer(roi=0.45, margin=10)
pre_turn_left = False
pre_turn_right = False
obstacle_time = 0.0


def callback(data):
    global end, i_left, i_right, rects, ratio
    delta = time.time() - end
    if delta >= 0:
        try:
            global image, color_image
            # convert_data(data.data)
            # i_left = 0
            # i_right = 319
            time1 = time.time()
            img = bridge.imgmsg_to_cv2(data, "passthrough")
            # img = cv2.resize(img, (320, 160))
            # i_l, i_r = check_obstacle_new(img)
            # print i_l, i_r
            # cv2.rectangle(color_image[lower_y:upper_y, border_x:-border_x],(x,y),(x+w,y+h),(0,255,0),2)
            # check_obstacle(thresh1, 0.5, delta)
            # print left_obstacle, right_obstacle
            # time2 = time.time()
            # print time2 - time1
            # # cv2.imshow('depth_left', img[90:159,  :int(0.25*IMG_W)])
            ratio = 16
            depth_preprocess(img, ratio)  # x, y, w, h
            print(rects)
            # img = cv2.resize(img, (640, 480))
            # cv2.imshow('raw', color_image)
            # cv2.imshow('segment', image)
            # cv2.waitKey(1)
            end = time.time()
        except CvBridgeError, e:
            print e


def depth_preprocess(frame, ratio):
    global i_left, i_right, rects
    # time1 = time.time()
    # frame = np.float32(frame)
    # frame *= 255.0/65535
    # frame = np.uint8(frame)
    frame = preprocess_img(frame, ratio)
    frame = cv2.GaussianBlur(frame, (5, 5), 0)
    # print(frame)

    frame = find_object(frame, 40)

    frame = remove_noise(frame, k=5)
    rects = find_obstacles(frame)

    # cv2.imshow('thresh', frame * 255.)
    # cv2.waitKey(1)
    # frame = remove_road(frame, road_distance=600, padding=120)
    # frame = remove_far(frame, far_distance=1200)
    # frame = remove_noise(frame, k=3)
    # rects = find_obstacles(frame)
    i_left, i_right = get_restriction(rects)
    # time2 = time.time()
    # print time2 - time1


def get_restriction(rects):
    x_left = IMG_W // 2
    x_right = IMG_W // 2
    for rect in rects:
        x, y, w, h = rect
        # print(rect)
        x = x * 8
        w = w * 8
        # cv2.rectangle(image, (x, 0), (x+w, IMG_H), (0, 255, 0), 1)

        if x > IMG_W // 2:
            if x > x_right:
                x_right = x

        if (x + w) < IMG_W // 2:
            if (x + w) < x_left:
                x_left = x + w

        if x < IMG_W // 2 and x + w > IMG_W / 2:
            if IMG_W / 2 - x > x + w - IMG_W / 2:
                x_left = x + w
                x_right = IMG_W - 1
            else:
                x_left = 0
                x_right = x

    if x_right != IMG_W // 2:
        i_right = x_right
    else:
        i_right = IMG_W - 1

    if x_left != IMG_W // 2:
        i_left = x_left
    else:
        i_left = 0
    return i_left, i_right


def check_obstacle_new(img):
    IMG_H, IMG_W = img.shape
    lower_y = int(3 * IMG_H / 16)
    upper_y = int(11 * IMG_H / 16)
    border_x = int(0.15 * IMG_W)
    img = img[lower_y:upper_y, border_x:-border_x]
    img = np.float32(img)
    img *= 255.0 / 65535
    img = np.uint8(img)
    img = cv2.medianBlur(img, 5)
    left_obstacle = False
    right_obstacle = False
    middle = (IMG_W - 2 * border_x) / 2
    ratio = 0.5
    # img *= 10
    ret, thresh1 = cv2.threshold(img, 3, 255, cv2.THRESH_BINARY_INV)
    _, contours, _ = cv2.findContours(thresh1, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)
    for contour in contours:
        if left_obstacle and right_obstacle:
            break
        if cv2.contourArea(contour) > 1500:
            x, y, w, h = cv2.boundingRect(contour)
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
    cv2.imshow('threshold', thresh1)
    if left_obstacle and not right_obstacle:
        return ratio * IMG_W, IMG_W - 1
    elif right_obstacle and not left_obstacle:
        return 0, ratio * IMG_W
    else:
        return 0, IMG_W - 1


def callback_image(data):
    global i_left, i_right, image, color_image
    img = convert_data_to_image(data.data)
    cv2.line(img, (i_left, 0), (i_left, 159), (0, 255, 0), 2)
    cv2.line(img, (i_right, 0), (i_right, 159), (255, 0, 0), 2)
    # cv2.line(img, (0, int(6 * 160 / 16)), (0, int(6 * 160 / 16)), (255, 0, 0), 2)
    # cv2.line(img, (0, int(12 * 160 / 16)), (319, int(12 * 160 / 16)), (255, 0, 0), 2)
    color_image = img
    # res = model.predict(np.expand_dims(img / 255., axis=0))
    # res = np.argmax(res, axis=3)[0]
    # res = res * 255.
    #
    # _, _, res = s2s.get_steer(img, res, 0, 0, i_left, i_right)
    # image = res

    # print image.shape

    cv2.imshow('image', img)
    cv2.waitKey(1)


def check_obstacle(img, threshold, interval):
    global obstacle_time
    IMG_H, IMG_W = img.shape
    global i_left, i_right
    left_obs = False
    right_obs = False
    i_left = 0
    i_right = IMG_W - 1
    i = int(0.5 * IMG_W)
    range = int(0.05 * IMG_W)
    lower_y = 0
    upper_y = IMG_H
    size = (upper_y - lower_y) * range
    max_value = 255
    ratio = 0.5

    if obstacle_time >= 0.0 and obstacle_time <= 2.0:
        obstacle_time += interval
        return

    obstacle_time = 0.0
    while i - range >= 0:
        if left_obs and right_obs:
            break
        if not left_obs:
            check = img[lower_y:upper_y, i - range:i]
            left_check = np.sum(check) / (max_value * size)
            if left_check <= threshold:
                left_obs = True
                i_left = i
        if not right_obs:
            check = img[lower_y:upper_y, IMG_W - i: IMG_W - i + range]
            right_check = np.sum(check) / (max_value * size)
            if right_check <= threshold:
                right_obs = True
                i_right = IMG_W - i
        i -= range
    if i_left >= i_right:
        i += range
        while np.sum(img[lower_y:upper_y, i_left - range:i_left]) / (
                max_value * size) <= threshold and i_left - range >= 0:
            i_left -= range
        while np.sum(img[lower_y:upper_y, i_right: i_right + range]) / (
                max_value * size) <= threshold and i_right + range <= IMG_W:
            i_right += range
        if i - i_left > i_right - i:
            i_left = int(ratio * IMG_W)
            i_right = IMG_W - 1
        elif i - i_left < i_right - i:
            i_right = int((1 - ratio) * IMG_W)
            i_left = 0
        else:
            i_left = 0
            i_right = IMG_W - 1
    # elif i_left >= 0.1 * IMG_W and i_right >= 0.9 * IMG_W:
    #     i_left = int(ratio * IMG_W)
    #     i_right = IMG_W - 1
    # elif i_right <= 0.9 * IMG_W and i_left <= 0.1 * IMG_W:
    #     i_right = int((1 - ratio) * IMG_W)
    #     i_left = 0
    # elif i_left >= 0.1 * IMG_W and i_right <= 0.9 * IMG_W:
    #     i_left = 0
    #     i_right = IMG_W - 1


def convert_data_to_image(data):
    arr = np.fromstring(data, np.uint8)
    image = cv2.imdecode(arr, 1)
    image = cv2.resize(image, (320, 160))
    return image


# def check_obstacle(image)

depth_sub = rospy.Subscriber('/camera/depth/image_raw', Image, callback, tcp_nodelay=True)
image_sub = rospy.Subscriber('/camera/rgb/image_raw/compressed', CompressedImage, callback_image, tcp_nodelay=True)
rospy.init_node('test')
rospy.spin()
