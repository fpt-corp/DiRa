import numpy as np
import time
import math
from collections import deque

total_time = 0
last = time.time()
pre_flag = 0
map = []
import cv2

total_distance = 0.0
last_speed = 0.0
first_tine = True
stored_time = 0.0


class SegmentToSteer():
    def __init__(self, square=7, margin=30, roi=1 / 3, size=1):
        self.square = square
        self.margin = margin
        self.size = size
        self.memory = deque(iterable=np.zeros(size, dtype=np.uint8), maxlen=size)
        self.speed_memory = deque(iterable=np.zeros(5, dtype=np.uint8), maxlen=5)
        self.direction_queue = deque(iterable=np.zeros(3, dtype=np.uint8), maxlen=3)
        self.roi = 1 - roi
        self.speed_max = 25
        self.speed_min = 15
        self.speed_brake = 15
        self.acc_threshold = 0.95
        # self.steer_pid = PID(Kp=self.p[0], Ki=self.p[1], Kd=self.p[2])
        # self.steer_pid.setWindup(50)
        self.error_proportional_ = 0.0
        self.error_integral_ = 0.0
        self.error_derivative_ = 0.0
        self.k_p = 0.5
        self.k_i = 0.00
        self.k_d = 0.00
        self.inc_p = 0.01
        self.inc_i = 0.0001
        self.inc_d = 0.01
        self.total_time_steer = 0.0
        self.total_time_steer_thresh = 14.0

    def pid(self, cte):
        self.error_integral_ += cte
        self.error_derivative_ = cte - self.error_proportional_
        self.error_proportional_ = cte
        return -(self.k_p * self.error_proportional_ + self.k_i * self.error_integral_ + self.k_d * self.error_derivative_)

    def check_future_road(self, img, roi, left_restriction, right_restriction):
        IMG_H, IMG_W = img.shape
        i = int(IMG_H * roi)
        border = int((self.square - 1) / 2)
        i_l = left_restriction + border
        i_r = right_restriction - border
        has_road = False
        turn_left = False
        turn_right = False
        while i_l < right_restriction + 1 - border:
            check = img[i - border: i + border + 1, i_l - border: i_l + border + 1]
            white = np.sum(check) / 255
            if white == self.square ** 2:
                if i_l <= self.margin:
                    turn_left = True
                has_road = True
                break
            i_l += (border + 1)
        while i_r > i_l:
            check = img[i - border: i + border + 1, i_r - border: i_r + border + 1]
            white = np.sum(check) / 255
            if white == self.square ** 2:
                if i_r >= IMG_W - self.margin:
                    turn_right = True
                has_road = True
                break
            i_r -= (border + 1)
        if turn_left and not turn_right:
            return -1, has_road
        elif turn_right and not turn_left:
            return 1, has_road
        return 0, has_road


    def get_point(self, img, flag, s, roi, left_restriction, right_restriction, has_road, road_property, total_time):
        IMG_H, IMG_W = img.shape
        i = int(IMG_H * roi)
        border = int((self.square - 1) / 2)
        i_l = left_restriction + border
        i_r = right_restriction - border
        turn_left = False
        turn_right = False
        while i_l < right_restriction + 1 - border:
            check = img[i - border: i + border + 1, i_l - border: i_l + border + 1]
            white = np.sum(check) / 255
            if white == self.square ** 2:
                if i_l <= self.margin:
                    turn_left = True
                break
            i_l += (border + 1)
        while i_r > i_l:
            check = img[i - border: i + border + 1, i_r - border: i_r + border + 1]
            white = np.sum(check) / 255
            if white == self.square ** 2:
                if i_r >= IMG_W - self.margin:
                    turn_right = True
                break
            i_r -= (border + 1)

        if (turn_left and turn_right and total_time > self.total_time_steer_thresh):
            flag = 1
            # flag = self.get_direction()
        if (turn_left and turn_right and flag == 1) or ((turn_right and not turn_left and flag != -1) and not has_road):
            # re phai
            self.direction_queue.append(2)
            while img[i][i_r] == 255 and i >= 0:
                i -= 1
            return i + 1, i_r
        elif (turn_left and turn_right and flag == -1) or ((turn_left and not turn_right and flag != 1) and not has_road):
            # re trai
            self.direction_queue.append(0)
            while img[i][i_l] == 255 and i >= 0:
                i -= 1
            return i + 1, i_l
        self.direction_queue.append(1)
        # di thang
        return i, int((i_l + i_r) / 2)

    def get_flag(self):
        arr = np.asarray(self.memory, np.int8)
        return np.bincount(arr).argmax() - 1

    def get_direction(self):
        arr = np.asarray(self.direction_queue, np.int8)
        return np.bincount(arr).argmax() - 1

    def mean_speed_queue(self):
        mean = 0
        for x in self.speed_memory:
            mean += x
        return mean / len(self.speed_memory)

    def write_image(self, raw, label, x, y, steer):
        raw[label == 255] = [128, 64, 128]
        font = cv2.FONT_HERSHEY_DUPLEX
        bottomLeftCornerOfText = (180, 158)
        fontScale = 0.5
        fontColor = (0, 255, 0)
        lineType = 1
        cv2.putText(raw, str(steer.__format__(".0f")), bottomLeftCornerOfText, font, fontScale, fontColor, lineType, 0)
        cv2.line(raw, (x, y), (160, 160), fontColor, thickness=2)
        raw = cv2.resize(raw, (480, 240))
        return raw

    def write_vector(self, img, x, y, steer):
        IMG_H, IMG_W = img.shape
        font = cv2.FONT_HERSHEY_DUPLEX
        bottomLeftCornerOfText = (180, 158)
        fontScale = 0.5
        fontColor = (0, 255, 0)
        lineType = 1
        cv2.putText(img, str(steer.__format__(".0f")), bottomLeftCornerOfText, font, fontScale, fontColor, lineType, 0)
        cv2.line(img, (int(IMG_W / 2), IMG_H), (x, y), (0, 0, 0), 1)
        return img

    def get_steer(self, raw, label, flag, s, left_restriction, right_restriction):
        global pre_flag
        global total_time
        global last
        global first_tine
        global stored_time
        global last_speed
        IMG_H, IMG_W = label.shape
        interval = time.time() - last
        last = time.time()
        roi = self.roi
        self.total_time_steer += interval
        if total_time > 0 and total_time < 3.5:
            if flag != pre_flag and flag != 0:
                total_time = 0
                pre_flag = flag
            total_time += interval
            current_flag = pre_flag
        else:
            total_time = 0
            # self.memory.append(flag+1)
            current_flag = flag
            # current_flag = self.get_flag()
            if current_flag != 0:
                pre_flag = current_flag
                total_time += interval
            else:
                pre_flag = current_flag
        # print 'current flag ', str(current_flag)
        road_property, has_road = self.check_future_road(label, 0.45, 0, IMG_W - 1)
        y, x = self.get_point(label, current_flag, s, roi, left_restriction, right_restriction, has_road, road_property, self.total_time_steer)
        # y, x = self.get_point_2(label, current_flag)

        while label[y][x] == 0 and roi < 0.9:
            roi += 0.05
            y, x = self.get_point(label, current_flag, s, roi, left_restriction, right_restriction, has_road, road_property, self.total_time_steer)

        # x = max(0, min(x + self.check_obstacle(label, x, y, 1), 319))
        # x = get_cte(label, current_flag, self.square, 1)
        # steer = np.arctan((x - IMG_W / 2 + 1) / (IMG_H - float(y))) * 57.32
        steer = -self.pid(x - IMG_W / 2 + 1)
        steer = np.sign(steer) * min(60, abs(steer))
        # steer = self.last_steer * self.beta + steer * (1 - self.beta)
        if s > self.acc_threshold:
            # print "accuracy ", str(s)
            self.speed_memory.append(min(self.speed_brake / (s ** 5), self.speed_max))
        elif left_restriction >= 0.5 * IMG_W or right_restriction <= 0.5 * IMG_W:
            self.speed_memory.append(self.speed_min)
        else:
            self.speed_memory.append((self.speed_min - self.speed_max) * steer**2/3600 + self.speed_max)
        speed = self.mean_speed_queue()

        # label = self.write_vector(label, x, y, steer)
        # speed = self.speed_max
        # label = self.write_vector(label, x, y, steer)
        return speed, steer, label
