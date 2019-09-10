#!/usr/bin/env python
import cv2
import numpy as np

# import rospy


class DepthProcessor:
    def __init__(self):
        self.SCALE = 10000  # == 1 meter
        self.MAX_UINT16 = 65536
        self.counter = 0
        self.area_130 = 130
        self.area_110 = 110
        self.middle_screen = (140, 180)

    def remove_ground(self, gray_img, column, row, n_row, lower, upper):
        """
        Remove the ground in gray image
        :param gray_img: input gray image 16 bit 1 channel
        :param column: column pixel
        :param row: row pixel
        :param n_row: number of rows to check
        :param lower: lower threshold (omit value which is less than T1)
        :param upper: upper threshold (omit value which is greater than T2)
        :return: pixel's value (0 if it's ground, otherwise)
        """
        if gray_img[row][column] > lower or gray_img[row][column] < upper:
            return 0
        if int(gray_img[row][column]) - int(gray_img[row + n_row][column]) >= 1:
            for i in range(n_row):
                if int(gray_img[row + i][column]) - int(gray_img[row + i + 1][column]) < 0:
                    return gray_img[row][column]
            return 0
        else:
            return gray_img[row][column]

    def resize_np(self, img_np, percent):
        """
        Resize image
        :param img_np: input any type of np_image
        :param percent: percent to resize image
        :return: image after being resized
        """
        h, w = img_np.shape
        w = int(w * percent)
        h = int(h * percent)
        resized_img = cv2.resize(img_np, (w, h))
        return resized_img

    def find_nearest_object(self, bounding_boxes):
        """
        Find the nearest object in one side (left or right)
        :param bounding_boxes: list bbox of objects in one side
        Example [(x1,y1,w1,h1), ..., (xn,yn,wn,hn)]
        :return: bbox of nearest object - (xi,yi,wi,hi)
        """
        b_new = list(bounding_boxes[i][1] + bounding_boxes[i][3] for i in range(len(bounding_boxes)))
        index = np.argmax(np.array(b_new))
        nearest_obstacle = bounding_boxes[index]
        return nearest_obstacle

    def find_danger_zone(self, obstacle_left, obstacle_right):
        """
        _____________________  danger_zone
        |              |
        V______________V
        |   |      |   |
        |   | bbox |   |
        |___|______|___|
        Find the danger zone after finding obstacles
        :param obstacle_left: bbox of the nearest left-obstacle (tuple) (x,y,w,h)
        :param obstacle_right: bbox of the nearest right-obstacle (tuple) (x,y,w,h)
        :return: tuple danger zone (a,b)
        # (0,0) : No obstacle
        # (-1, middle_point) : 2 obstacles
        # (n,m) : left_edge, right_edge of obstacle
        """
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
            if self.middle_screen[0] < center_object < self.middle_screen[1]:
                danger_zone = (x - self.area_110, x + w + self.area_110)
            else:
                danger_zone = (x - self.area_130, x + w + self.area_130)
        if obstacle_right != 0 and obstacle_left == 0:
            (x, y, w, h) = obstacle_right
            center_object = int((x + x + w) / 2)
            if self.middle_screen[0] < center_object < self.middle_screen[1]:
                danger_zone = (x - self.area_110, x + w + self.area_110)
            else:
                danger_zone = (x - self.area_130, x + w + self.area_130)
        return danger_zone

    def detect_obstacle_without_danger_zone(self, img_np, n_row=2, lower=15000, upper=1000, min_width=40, min_height=40):
        """
        Detect obstacle
        :param img_np: input image in gray scale 16 bit 1 channel
        :param n_row: number of rows to check
        :param lower: lower threshold (omit value which is less than T1)
        :param upper: upper threshold (omit value which is greater than T2)
        :param min_width: minimum of bbox's width
        :param min_height: minimum of bbox's height
        :return: danger_zone (tuple) (a,b)
        # (0,0) : No obstacle
        # (-1, middle_point) : 2 obstacles -> go through between them (middle_point)
        # (n,m) : left_edge, right_edge of one obstacle
        """
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

        height, width = gray_img.shape

        # remove floor and wall far away...
        for x in range(width):
            for y in range(height):
                # if gray_img[y][x] > T1 or gray_img[y][x] < T2:
                   # gray_img[y][x] = 0
                if y < height - n_row:
                    gray_img[y][x] = self.remove_ground(gray_img, x, y, n_row, lower, upper)
                else:
                    gray_img[y][x] = 0

        # Open
        kernel_open = np.ones((3, 3), np.uint8)
        gray_img = cv2.morphologyEx(gray_img, cv2.MORPH_OPEN, kernel_open)

        # resize
        gray_img = self.resize_np(gray_img, 8)
        gray_uint8 = cv2.convertScaleAbs(gray_img)
        _, contours, hierarchy = cv2.findContours(gray_uint8, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

        img_rgb_np = cv2.cvtColor(img_np, cv2.COLOR_GRAY2RGB)
        # img_rgb_np = cv2.cvtColor(gray_img, cv2.COLOR_GRAY2RGB)
        # cv2.drawContours(img_rgb_np, contours, -1, (MAX_UINT16, 0, 0), 2)
        # cv2.imshow('contours', img_rgb_np)

        bbox_left = []
        bbox_right = []
        # print('number of contours', len(contours))
        for contour in contours:
            (x, y, w, h) = cv2.boundingRect(contour)

            # print(w,h)
            if h > min_height:
                cv2.rectangle(img_rgb_np, (x, y), (x + w, y + h), (0, self.MAX_UINT16, 0), 2)
                # draw danger zone
                cv2.rectangle(img_rgb_np, (x - self.area_110, y), (x + w + self.area_110, y + h),
                              (self.MAX_UINT16, 0, 0), 2)

                center_x = x + int(w / 2)

                if center_x < 160:
                    bbox_left.append((x, y, w, h))
                else:
                    bbox_right.append((x, y, w, h))

        return bbox_left, bbox_right

    def detect_obstacle_with_danger_zone(self, img_np):
        """
        Return the final result to control car after combining module detect-obstacle-without-danger-zone and find-danger-zone
        :param img_np: numpy depth image (grayscale)
        :return: tuple danger zone (a,b)
        # (0,0) : No obstacle
        # (-1, middle_point) : 2 obstacles
        # (n,m) : left_edge, right_edge of obstacle
        """

        bbox_left, bbox_right = self.detect_obstacle_without_danger_zone(img_np)
        obstacle_left = obstacle_right = 0
        if len(bbox_left) > 0:
            obstacle_left = self.find_nearest_object(bbox_left)
        if len(bbox_right) > 0:
            obstacle_right = self.find_nearest_object(bbox_right)

        danger_zone = self.find_danger_zone(obstacle_left, obstacle_right)

        return danger_zone
