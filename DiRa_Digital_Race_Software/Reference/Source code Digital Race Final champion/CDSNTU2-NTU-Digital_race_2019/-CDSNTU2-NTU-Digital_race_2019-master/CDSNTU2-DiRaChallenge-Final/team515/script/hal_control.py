#!/usr/bin/python2
import time
import rospy
from std_msgs.msg import Bool, String, Float32

preBt2Stt = False
preBt3Stt = False
MIN_SPEED = 0
MAX_SPEED = 24
dem = 0

class Hal:
    def __init__(self):
        self.speed = 0.0
        self.preSpeed = self.speed
        self.ss_stt = True
        self.loTrinh = None

        self.subscriber = rospy.Subscriber("/bt1_status", Bool, self.bt1callback, queue_size=10)
        self.subscriber = rospy.Subscriber("/bt2_status", Bool, self.bt2callback, queue_size=10)
        self.subscriber = rospy.Subscriber("/bt3_status", Bool, self.bt3callback, queue_size=10)
        self.subscriber = rospy.Subscriber("/bt4_status", Bool, self.bt4callback, queue_size=10)
        self.ssStt = rospy.Subscriber("/ss_status", Bool, self.ssCallback, queue_size=10)
        self.lcd_pub = rospy.Publisher("/lcd_print", String, queue_size=10)
        
        self.subscriber = rospy.Subscriber('route', String, self.LoTrinh, queue_size=1)

    def LoTrinh(self, data):
        self.loTrinh = data.data

    def bt1callback(self, bt1_data):
        if(bt1_data.data == True):
            self.speed = 0.0

    def bt2callback(self, bt2_data):
        global preBt2Stt
        if(bt2_data.data != preBt2Stt and bt2_data.data == True):
            if(self.speed < 10):
                self.speed = 10.0
            else:
                self.speed += 2.0
        if(self.speed > MAX_SPEED):
            self.speed = MAX_SPEED
        preBt2Stt = bt2_data.data

    def bt3callback(self, bt3_data):
        global preBt3Stt
        if(bt3_data.data != preBt3Stt and bt3_data.data == True):
            if(self.speed <= 10):
                self.speed = 0.0
            else:
                self.speed -= 1.0
        preBt3Stt = bt3_data.data

    def bt4callback(self, bt4_data):
        if(bt4_data.data == True):
            self.speed = 15.0

    def ssCallback(self, ss_data):
        self.ss_stt = ss_data.data

    def pubLcdPrint(self):
        global dem
        dem += 1
        if(dem == 10):
            string = "0:0:speed  " + str(self.speed)
            self.lcd_pub.publish(string)
            self.lcd_pub.publish("0:2:      CDSNTU2")
            if (self.loTrinh is not None):
                string1 = "0:3:    " + str(self.loTrinh)
                self.lcd_pub.publish(string1)
            dem = 0
    
