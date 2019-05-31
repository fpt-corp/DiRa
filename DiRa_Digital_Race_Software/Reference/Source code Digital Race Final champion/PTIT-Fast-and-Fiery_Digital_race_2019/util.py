#!/usr/bin/env python
import sys
import rospy
import cv2
import numpy as np
import os
import time
from rospkg import RosPack
from team107_node import Processor
from std_msgs.msg import String, Float32, Bool
from ssd import Detector
from rospy import ROSException
from model import Model

rospack = RosPack()


class Utilities:
    def __init__(self):
        # params
        self.count = 1
        self.clear_str = '0:0:                                                                                '
        self.is_configuring = False
        self.is_self_driving = False
        self.is_time_configuring = False
        self.first_load = True
        self.engine = None
        self.video_recorder = None
        self.path = rospack.get_path('team107') + '/scripts/'
        self.bt1_status = False
        self.bt2_status = False
        self.bt3_status = False
        self.bt4_status = False
        self.ss_status = False
        self.model = Model(self.path)
        self.sign_model = Detector(self.path)
        # ros subscribers and publishers
        # hal
        self.sub_bt1 = rospy.Subscriber('/bt1_status', Bool, self.bt1_callback, queue_size=1)
        self.sub_bt2 = rospy.Subscriber('/bt2_status', Bool, self.bt2_callback, queue_size=1)
        self.sub_bt3 = rospy.Subscriber('/bt3_status', Bool, self.bt3_callback, queue_size=1)
        self.sub_bt4 = rospy.Subscriber('/bt4_status', Bool, self.bt4_callback, queue_size=1)
        self.sub_ss = rospy.Subscriber('/ss_status', Bool, self.ss_callback, queue_size=1)
        self.pub_lcd = rospy.Publisher('/lcd_print', String, queue_size='5')
        # car controller
        self.sub_img = None
        rospy.init_node('team107')
        print 'Finish initializing'
        # finish

    # setup callbacks
    def bt1_callback(self, data):
        self.bt1_status = data.data

    def bt2_callback(self, data):
        self.bt2_status = data.data

    def bt3_callback(self, data):
        self.bt3_status = data.data

    def bt4_callback(self, data):
        self.bt4_status = data.data

    def ss_callback(self, data):
        self.ss_status = data.data
        self.check_btns()

    def image_callback(self, ros_data):
        image = self.convert_to_image(ros_data.data)
        self.video_recorder.write(image)
        cv2.imshow('image', image)
        cv2.waitKey(1)

    # button interactions
    def check_btns(self):
        if  not self.is_self_driving:
            try:
                if self.first_load:
                    self.pub_lcd.publish(self.clear_str)
                    self.pub_lcd.publish('0:0:welcome Fast n Fiery')
                    self.first_load = False
            except ROSException:
                pass
            if not self.bt1_status and self.bt2_status and not self.bt3_status and not self.bt4_status:
                self.setup_engine()
                self.engine_control()
        elif self.is_self_driving:
            self.engine_control()

    # engine functions
    def setup_engine(self):
        self.is_self_driving = True
        self.is_recording = False
        self.engine = Processor(model=self.model, sign_model=self.sign_model)
        self.print_car_stats()
        time.sleep(1)

    def engine_control(self):
        if not self.is_configuring:
            if self.bt1_status and self.bt2_status and self.bt3_status and not self.bt4_status:
                self.turn_off_engine()
            elif self.bt1_status and not self.bt2_status and not self.bt3_status and not self.bt4_status:
                self.change_max_speed(1)
            elif not self.bt1_status and self.bt2_status and not self.bt3_status and not self.bt4_status:
                self.change_min_speed(1)
            elif not self.bt1_status and not self.bt2_status and self.bt3_status and not self.bt4_status:
                self.setup_configuration()
                self.configuration_control()
            elif self.bt1_status and not self.bt2_status and not self.bt3_status and self.bt4_status:
                self.change_max_speed(-1)
            elif not self.bt1_status and self.bt2_status and not self.bt3_status and self.bt4_status:
                self.change_min_speed(-1)
        elif self.is_configuring:
            self.configuration_control()

    def change_max_speed(self, amount):
        self.engine.s2s.speed_max += amount
        self.print_car_stats()
        time.sleep(0.5)

    def change_min_speed(self, amount):
        self.engine.s2s.speed_min += amount
        self.print_car_stats()
        time.sleep(0.5)

    def turn_off_engine(self):
        self.is_self_driving = False
        del self.engine
        self.engine = None
        self.pub_lcd.publish(self.clear_str)
        self.pub_lcd.publish("0:0:Engine turned off")
        self.first_time = True
        time.sleep(1)

    #change brake config
    def setup_configuration(self):
        self.is_configuring = True
        self.print_brake_stats()
        time.sleep(1)

    def configuration_control(self):
        if not self.is_time_configuring:
            if self.bt1_status and not self.bt2_status and not self.bt3_status and not self.bt4_status:
                self.change_brake_speed(1)
            elif not self.bt1_status and self.bt2_status and not self.bt3_status and not self.bt4_status:
                self.change_accuracy_threshold(0.01)
            elif not self.bt1_status and not self.bt2_status and self.bt3_status and not self.bt4_status:
                self.setup_time_configuration()
                self.time_control()
            elif not self.bt1_status and not self.bt2_status and self.bt3_status and self.bt4_status:
                self.exit_configuration()
            elif self.bt1_status and not self.bt2_status and not self.bt3_status and self.bt4_status:
                self.change_brake_speed(-1)
            elif not self.bt1_status and self.bt2_status and not self.bt3_status and self.bt4_status:
                self.change_accuracy_threshold(-0.01)
        elif self.is_time_configuring:
            self.time_control()

    def exit_configuration(self):
        self.is_configuring = False
        self.pub_lcd.publish(self.clear_str)
        self.print_car_stats()
        time.sleep(1)

    def change_brake_speed(self, amount):
        self.engine.s2s.speed_brake += amount
        self.print_brake_stats()
        time.sleep(0.5)

    def change_accuracy_threshold(self, amount):
        self.engine.s2s.acc_threshold = max(min(self.engine.s2s.acc_threshold + amount, 1.0), 0.0)
        self.print_brake_stats()
        time.sleep(0.5)

    #change time config
    def setup_time_configuration(self):
        self.is_time_configuring = True
        self.print_time_stats()
        time.sleep(1)

    def time_control(self):
        if self.bt1_status and not self.bt2_status and not self.bt3_status and not self.bt4_status:
            self.change_obstacle_stop_time(0.1)
        elif not self.bt1_status and self.bt2_status and not self.bt3_status and not self.bt4_status:
            self.change_sign_stop_time(0.1)
        elif self.bt1_status and not self.bt2_status and not self.bt3_status and self.bt4_status:
            self.change_obstacle_stop_time(-0.1)
        elif not self.bt1_status and self.bt2_status and not self.bt3_status and self.bt4_status:
            self.change_sign_stop_time(-0.1)
        elif not self.bt1_status and not self.bt2_status and self.bt3_status and self.bt4_status:
            self.exit_time_configuration()

    def exit_time_configuration(self):
        self.is_time_configuring = False
        self.pub_lcd.publish(self.clear_str)
        self.print_brake_stats()
        time.sleep(1)

    def change_obstacle_stop_time(self, amount):
        self.engine.total_time_thresh += amount
        self.print_time_stats()
        time.sleep(0.5)

    def change_sign_stop_time(self, amount):
        self.engine.s2s.total_time_steer_thresh += amount
        self.print_time_stats()
        time.sleep(0.5)

    def convert_to_image(self, data):
        arr = np.fromstring(data, np.uint8)
        image = cv2.imdecode(arr, 1)
        image = cv2.resize(image, (320, 160))
        return image

    def free_space(self, path):
        st = os.statvfs(path)
        free = st.f_bavail * st.f_frsize
        return "free " + str(free / (1000 * 1000)) + "MB"

    def print_car_stats(self):
        self.pub_lcd.publish(self.clear_str)
        self.pub_lcd.publish('0:0:max ' + str(self.engine.s2s.speed_max))
        self.pub_lcd.publish('7:0:min ' + str(self.engine.s2s.speed_min))

    def print_brake_stats(self):
        self.pub_lcd.publish(self.clear_str)
        self.pub_lcd.publish('0:0:brk ' + str(self.engine.s2s.speed_brake))
        self.pub_lcd.publish('7:0:acc ' + str(self.engine.s2s.acc_threshold))

    def print_time_stats(self):
        self.pub_lcd.publish(self.clear_str)
        self.pub_lcd.publish('0:0:obs ' + str(self.engine.total_time_thresh))
        self.pub_lcd.publish('8:0:sign ' + str(self.engine.s2s.total_time_steer_thresh))

def main(args):
    util = Utilities()
    try:
        rospy.spin()
    except:
        print("Shutting down")


if __name__ == '__main__':
    main(sys.argv)
