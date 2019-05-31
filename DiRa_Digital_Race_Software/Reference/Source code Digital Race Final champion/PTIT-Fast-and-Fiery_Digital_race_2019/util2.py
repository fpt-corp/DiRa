#!/usr/bin/env python
import sys
import rospy
import cv2
import numpy as np
import os
import time
import tensorflow as tf
from tensorflow.keras.models import Sequential
from tensorflow.keras.layers import *
from tensorflow.keras import backend as K
from tensorflow.keras.models import Model
from tensorflow.keras import applications
from rospkg import RosPack
from team107_node import Processor
from sensor_msgs.msg import CompressedImage
from std_msgs.msg import String, Float32, Bool
from rospy import ROSException
from model import Model
from ssd import Detector

rospack = RosPack()


class Utilities:
    def __init__(self):
        # params
        self.count = 1
        self.clear_str = '0:0:                                                                                '
        self.is_recording = False
        self.is_self_driving = False
        self.first_load = True
        self.engine = None
        self.video_recorder = None
        self.path = rospack.get_path('team107') + '/scripts/'
        self.bt1_status = False
        self.bt2_status = False
        self.bt3_status = False
        self.bt4_status = False
        self.ss_status = False
        self.model = Model(self.path + 'TensorRT_FP16.pb')
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
        if not self.is_recording and not self.is_self_driving:
            try:
                if self.first_load:
                    self.pub_lcd.publish(self.clear_str)
                    self.pub_lcd.publish('0:0:welcome Fast n Fiery')
                    self.first_load = False
            except ROSException:
                pass
            if self.bt1_status and not self.bt2_status and not self.bt3_status and not self.bt4_status:
                self.setup_record()
                self.record_control()
            elif not self.bt1_status and self.bt2_status and not self.bt3_status and not self.bt4_status:
                self.setup_pid()
                self.pid_control()
        elif self.is_recording:
            self.record_control()
        elif self.is_self_driving:
            self.pid_control()

    # record functions
    def setup_record(self):
        self.is_recording = True
        self.is_self_driving = False
        self.bt1_status = False
        fourcc = cv2.VideoWriter_fourcc(*'XVID')
        self.video_recorder = cv2.VideoWriter(self.path + '/segment_data/' + str(time.time()) + '.mp4', fourcc, 20.0,
                                              (640, 480))
        self.pub_lcd.publish(self.clear_str)
        self.sub_img = rospy.Subscriber('/camera/rgb/image_raw/compressed', CompressedImage, self.image_callback,
                                        queue_size=1)
        self.pub_lcd.publish('0:0:' + self.free_space('/home'))
        print 'start recording'
        time.sleep(1)

    def record_control(self):
        if self.bt1_status and self.bt2_status and self.bt3_status and not self.bt4_status:
            self.turn_off_record()

    def turn_off_record(self):
        self.is_recording = False
        self.sub_img.unregister()
        self.video_recorder.release()
        self.pub_lcd.publish(self.clear_str)
        self.pub_lcd.publish("0:0:Finish recording data")
        self.first_load = True
        print 'turnning off'
        time.sleep(1)

    # engine functions
    def setup_engine(self):
        self.is_self_driving = True
        self.is_recording = False
        self.engine = Processor(model=self.model)
        self.print_car_stats(self.engine.s2s.speed_max, self.engine.s2s.speed_min, self.engine.s2s.roi)
        time.sleep(1)

    def engine_control(self):
        if self.bt1_status and self.bt2_status and self.bt3_status and not self.bt4_status:
            self.turn_off_engine()
        elif self.bt1_status and not self.bt2_status and not self.bt3_status and not self.bt4_status:
            self.change_max_speed(1)
        elif not self.bt1_status and self.bt2_status and not self.bt3_status and not self.bt4_status:
            self.change_min_speed(1)
        elif not self.bt1_status and not self.bt2_status and self.bt3_status and not self.bt4_status:
            self.change_roi(0.05)
        elif self.bt1_status and not self.bt2_status and not self.bt3_status and self.bt4_status:
            self.change_max_speed(-1)
        elif not self.bt1_status and self.bt2_status and not self.bt3_status and self.bt4_status:
            self.change_min_speed(-1)
        elif not self.bt1_status and not self.bt2_status and self.bt3_status and self.bt4_status:
            self.change_roi(-0.05)

    def change_max_speed(self, amount):
        self.engine.s2s.speed_max += amount
        self.print_car_stats(self.engine.s2s.speed_max, self.engine.s2s.speed_min, self.engine.s2s.roi)
        time.sleep(0.5)

    def change_min_speed(self, amount):
        self.engine.s2s.speed_min += amount
        self.print_car_stats(self.engine.s2s.speed_max, self.engine.s2s.speed_min, self.engine.s2s.roi)
        time.sleep(0.5)

    def change_roi(self, amount):
        change = self.engine.s2s.roi + amount
        self.engine.s2s.roi = max(0.1, min(change, 0.9))
        self.print_car_stats(self.engine.s2s.speed_max, self.engine.s2s.speed_min, self.engine.s2s.roi)
        time.sleep(0.5)

    def turn_off_engine(self):
        self.is_self_driving = False
        del self.engine
        self.engine = None
        self.pub_lcd.publish(self.clear_str)
        self.pub_lcd.publish("0:0:Engine turned off")
        self.first_time = True
        time.sleep(1)

    #pid control
    def setup_pid(self):
        self.is_self_driving = True
        self.is_recording = False
        self.engine = Processor(model=self.model, sign_model=self.sign_model)
        self.print_pid_stats()
        time.sleep(1)

    def pid_control(self):
        if self.bt1_status and self.bt2_status and self.bt3_status and not self.bt4_status:
            self.turn_off_pid()
        elif self.bt1_status and not self.bt2_status and not self.bt3_status and not self.bt4_status:
            self.change_kp(self.engine.s2s.inc_p)
        elif not self.bt1_status and self.bt2_status and not self.bt3_status and not self.bt4_status:
            self.change_ki(self.engine.s2s.inc_i)
        elif not self.bt1_status and not self.bt2_status and self.bt3_status and not self.bt4_status:
            self.change_kd(self.engine.s2s.inc_d)
        elif self.bt1_status and not self.bt2_status and not self.bt3_status and self.bt4_status:
            self.change_kp(-self.engine.s2s.inc_p)
        elif not self.bt1_status and self.bt2_status and not self.bt3_status and self.bt4_status:
            self.change_ki(-self.engine.s2s.inc_i)            
        elif not self.bt1_status and not self.bt2_status and self.bt3_status and self.bt4_status:
            self.change_kd(-self.engine.s2s.inc_d)            

    def change_kp(self, amount):
        self.engine.s2s.k_p += amount
        self.print_pid_stats()
        time.sleep(0.5)

    def change_ki(self, amount):
        self.engine.s2s.k_i += amount
        self.print_pid_stats()
        time.sleep(0.5)

    def change_kd(self, amount):
        self.engine.s2s.k_d += amount
        self.print_pid_stats()
        time.sleep(0.5)

    def turn_off_pid(self):
        self.is_self_driving = False
        del self.engine
        self.engine = None
        self.pub_lcd.publish(self.clear_str)
        self.pub_lcd.publish("0:0:Engine turned off")
        self.first_time = True
        time.sleep(1)

    # utility functions
    def load_model(self):
        from tensorflow.keras.models import model_from_json
        json_file = open(self.path+'model.json', 'r')
        loaded_model_json = json_file.read()
        loaded_model = model_from_json(loaded_model_json)
        loaded_model.load_weights(self.path+"model.h5")
        loaded_model._make_predict_function()
        print ("model 1M loaded")
        return loaded_model

    def load_model_segment(self):
        mbl = applications.mobilenet.MobileNet(weights=None, include_top=False, input_shape=(160, 320, 3))
        x = mbl.output
        model_tmp = Model(inputs=mbl.input, outputs=x)
        layer5, layer8, layer13 = model_tmp.get_layer('conv_pw_5_relu').output, model_tmp.get_layer(
            'conv_pw_8_relu').output, model_tmp.get_layer('conv_pw_13_relu').output

        fcn14 = Conv2D(filters=2, kernel_size=1, name='fcn14')(layer13)
        fcn15 = Conv2DTranspose(filters=layer8.get_shape().as_list()[-1], kernel_size=4, strides=2, padding='same',
                                name='fcn15')(fcn14)
        fcn15_skip_connected = Add(name="fcn15_plus_vgg_layer8")([fcn15, layer8])
        fcn16 = Conv2DTranspose(filters=layer5.get_shape().as_list()[-1], kernel_size=4, strides=2, padding='same',
                                name="fcn16_conv2d")(fcn15_skip_connected)
        # Add skip connection
        fcn16_skip_connected = Add(name="fcn16_plus_vgg_layer5")([fcn16, layer5])
        # Upsample again
        fcn17 = Conv2DTranspose(filters=2, kernel_size=16, strides=(8, 8), padding='same', name="fcn17",
                                activation="softmax")(fcn16_skip_connected)
        m = Model(inputs=mbl.input, outputs=fcn17)
        m.load_weights(self.path + 'model-mobilenet-iter2-pretrain-data-bdd.h5')
        m.predict(np.zeros((1, 160, 320, 3), dtype=np.float32))
        print("Model loaded")
        return m

    def convert_to_image(self, data):
        arr = np.fromstring(data, np.uint8)
        image = cv2.imdecode(arr, 1)
        image = cv2.resize(image, (320, 160))
        return image

    def free_space(self, path):
        st = os.statvfs(path)
        free = st.f_bavail * st.f_frsize
        return "free " + str(free / (1000 * 1000)) + "MB"

    def print_car_stats(self, speed_max, speed_min, roi):
        self.pub_lcd.publish(self.clear_str)
        self.pub_lcd.publish('0:0:max ' + str(speed_max))
        self.pub_lcd.publish('7:0:min ' + str(speed_min))
        self.pub_lcd.publish('0:2:roi ' + str(roi))

    def print_pid_stats(self):
        self.pub_lcd.publish(self.clear_str)
        self.pub_lcd.publish('0:0:Kp ' + str(self.engine.s2s.k_p))
        self.pub_lcd.publish('9:0:Ki ' + str(self.engine.s2s.k_i))
        self.pub_lcd.publish('0:2:Kd ' + str(self.engine.s2s.k_d))


def main(args):
    util = Utilities()
    try:
        rospy.spin()
    except:
        print("Shutting down")


if __name__ == '__main__':
    main(sys.argv)
