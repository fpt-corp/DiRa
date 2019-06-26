#!/usr/bin/env python
import cv2
import numpy as np
import rospy


class DepthCamera:
    def __init__(self):
        self.SCALE = 10000  # ==1m
        self.MAX_UINT16 = 65536
        self.counter = 0
	rospy.Rate(10)
        self.area_130 = 130
        self.area_100 = 110
        print(cv2.__version__)
        # self.path = rospkg.RosPack.get_path('team105_detectsign')

    def ground(self, gray_img, x, y, n, T1, T2):
        if gray_img[y][x] > T1 or gray_img[y][x] < T2:
            return 0
        if int(gray_img[y][x]) - int(gray_img[y + n][x]) >= 1:
            for i in range(n):
                if int(gray_img[y + i][x]) - int(gray_img[y + i + 1][x]) < 0:
                    return gray_img[y][x]
            return 0
        else:
            return gray_img[y][x]

    def resize_np(self, img_np, percent):
        h, w = img_np.shape
        w = int(w * percent)
        h = int(h * percent)
        resized_img = cv2.resize(img_np, (w, h))
        return resized_img

    def find_nearest_object(self, bbox):
        b_new = list(bbox[i][1] + bbox[i][3] for i in range(len(bbox)))
        index = np.argmax(np.array(b_new))
        nearest_obstacle = bbox[index]
        return nearest_obstacle

    def find_danger_zone(self, obstacle_left, obstacle_right):
        # danger zone
        danger_zone = (0, 0)  # init
        # 2 objects
        if obstacle_left != 0 and obstacle_right != 0:
            (x_left, y_left, w_left, h_left) = obstacle_left
            (x_right, y_right, w_right, h_right) = obstacle_right

            # remove the further one if not parallel
            if (y_left + h_left) - (y_right + h_right) > 50:
                obstacle_right = 0
            elif (y_right + h_right) - (y_left + h_left) > 50:
                obstacle_left = 0

            # go through between them
            else:
                right_edge_object_left = x_left + w_left
                left_edge_object_right = x_right

                center_zone = int((right_edge_object_left + left_edge_object_right) / 2)
                danger_zone = (-1, center_zone)

        # single object
        if obstacle_left != 0 and obstacle_right == 0:
            (x, y, w, h) = obstacle_left
            center_object = int((x + x + w) / 2)
            if 100 < center_object < 220:
                danger_zone = (x - self.area_100, x + w + self.area_100)
            else:
                danger_zone = (x - self.area_130, x + w + self.area_130)
        if obstacle_right != 0 and obstacle_left == 0:
            (x, y, w, h) = obstacle_right
            center_object = int((x + x + w) / 2)
            if 100 < center_object < 220:
                danger_zone = (x - self.area_100, x + w + self.area_100)
            else:
                danger_zone = (x - self.area_130, x + w + self.area_130)
        return danger_zone

    def pre_processing_depth_img(self, img_np, n=2, T1=15000, T2=1000, min_width=35, min_height=35):
        # cv2.imwrite("./img_depth/raw/src_depth_"+str(self.counter)+".jpg", img_np)
        # resize
        gray_img = self.resize_np(img_np, 0.125)
        # cv2.imshow('src', gray_img)

        # cv2.waitKey(1)
	
        # CLOSE
        kernel_close = np.ones((3, 3))
        gray_img = cv2.morphologyEx(gray_img, cv2.MORPH_CLOSE, kernel_close)

        # DILATE
        kernel_dilate = np.ones((3, 3))
        gray_img = cv2.dilate(gray_img, kernel_dilate)
        # print(np.max(gray_img))
        # cv2.imshow('depth', gray_img)
        # cv2.waitKey()

        height, width = gray_img.shape
        # print(height, width)

        # remove floor and wall far away...
        for x in range(width):
            for y in range(height):
                # if gray_img[y][x] > T1 or gray_img[y][x] < T2:
                #    gray_img[y][x] = 0
                if y < height - n:
                    gray_img[y][x] = self.ground(gray_img, x, y, n, T1, T2)
                else:
                    gray_img[y][x] = 0

        # cv2.imshow('after remove floor', gray_img)

        # OPEN
        kernel_open = np.ones((3, 3), np.uint8)
        gray_img = cv2.morphologyEx(gray_img, cv2.MORPH_OPEN, kernel_open)
        # cv2.imshow('removed_ground_OPEN', gray_img)

        # CLOSE
        # kernel_close = np.ones((3, 3), np.uint8)
        # gray_img = cv2.morphologyEx(gray_img, cv2.MORPH_CLOSE, kernel_close)
        # cv2.imshow('removed_ground_CLOSE', gray_img)

        # resize
        gray_img = self.resize_np(gray_img, 8)
        # cv2.imwrite("/img_depth/img_processed/processed_"+str(self.counter)+".jpg", gray_img)
        # cv2.imshow('preprocessed', gray_img)

        # ret, thresh = cv2.threshold(gray_img, 10, 200, cv2.THRESH_BINARY)
        # cv2.imshow('bin', thresh)
        # print(np.max(gray_img))
        gray_uint8 = cv2.convertScaleAbs(gray_img)
        _,contours, hierarchy = cv2.findContours(gray_uint8, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

        img_RGB_np = cv2.cvtColor(img_np, cv2.COLOR_GRAY2RGB)
        # img_RGB_np = cv2.cvtColor(gray_img, cv2.COLOR_GRAY2RGB)
        # cv2.drawContours(img_RGB_np, contours, -1, (MAX_UINT16, 0, 0), 2)
        # cv2.imshow('contours', img_RGB_np)

        bbox_left = []
        bbox_right = []
        # print('number of contours', len(contours))
        for contour in contours:
            (x, y, w, h) = cv2.boundingRect(contour)

            # print(w,h)
            if h > min_height:
                cv2.rectangle(img_RGB_np, (x, y), (x + w, y + h), (0, self.MAX_UINT16, 0), 2)
                # draw danger zone
                cv2.rectangle(img_RGB_np, (x - self.area_100, y), (x + w + self.area_100, y + h),
                              (self.MAX_UINT16, 0, 0), 2)

                center_x = x + int(w / 2)

                if center_x < 160:
                    bbox_left.append((x, y, w, h))
                else:
                    bbox_right.append((x, y, w, h))

        # left - right
        obstacle_left = obstacle_right = 0
        if len(bbox_left) > 0:
            obstacle_left = self.find_nearest_object(bbox_left)
        if len(bbox_right) > 0:
            obstacle_right = self.find_nearest_object(bbox_right)

        danger_zone = self.find_danger_zone(obstacle_left, obstacle_right)

        #cv2.imshow('box', img_RGB_np)
        #cv2.waitKey(1)
        return danger_zone


if __name__ == '__main__':
    rospy.init_node('team105', anonymous=True)
    dc = DepthCamera()
    rospy.spin()
