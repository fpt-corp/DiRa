#!/usr/bin/python2
import rospy
from sensor_msgs.msg import Joy
from std_msgs.msg import Float32

"""
axex:
    0   left/right Axis stick left
    1   up/down Axis stick left
    2   left/right Axis stick right
    3   up/down Axis stick right
    4   cross key left/right
    5   cross key up/down
buttons:
    0   X
    1   A
    2   B
    3   Y
    4   LB
    5   RB
    6   LT
    7   RT
    8   BACK
    9   START
    10  stick click LEFT
    11  stick click right
"""

STEER_LIMIT = 60.0
MAX_SPEED = 20.0
MIN_SPEED = 0.0
speed_amount = 5.0

class JC:
    def __init__(self):
        self.pause = False
        self.speed = 0.0
        self.steer = 0.0
        self.stop = 0
        self.mode = 0
        self.change = 0
        self.recording = False
        self.subscriber = rospy.Subscriber("/joy", Joy, self.callBack, queue_size = 10)
    def callBack(self, ros_data):

        self.steer = round(-(ros_data.axes[2])*STEER_LIMIT, 3)
        #exit
        self.stop = ros_data.buttons[0]

        
        #record
        if(ros_data.buttons[6] == 1):
            self.recording = False
        if(ros_data.buttons[7] == 1):
            self.recording = True
        #mode switch
        if(ros_data.buttons[9] == 1):
            self.mode +=1
            if(self.mode == 2):
                self.mode = 0
        #mode 0
        if (self.mode == 0):
            if(ros_data.axes[5] == -1):
                '''self.speed = self.speed - speed_amount
                if(self.speed < 12.0 and self.speed > 0):
                    self.speed = 0.0
                if(self.speed < MIN_SPEED):
                    self.speed = MIN_SPEED'''
                self.speed = -15.0
            if(ros_data.axes[5] == 1):
                #self.speed = self.speed + speed_amount
                self.speed = 20.0
                if(self.speed < 12.0):
                    self.speed = 12.0
                if(self.speed > MAX_SPEED):
                    self.speed = MAX_SPEED
            if (ros_data.buttons[5] == 1):
                self.pause = True
                self.speed = 0.0
                self.recording = False
            else:
                self.pause = False
        #mode 1
        if (self.mode == 1):   
            if(ros_data.axes[1] > 0):
                self.speed = 20 * ros_data.axes[1]
            if(ros_data.axes[1] < 0):
                self.speed = 15.0 * ros_data.axes[1]
            if(ros_data.axes[1] == 0):
                self.speed = 0.0

        if (ros_data.buttons[3] == 1):
            self.speed = -100.0 
            
        #if(self.speed < 0.0):
            #self.speed = 0.0
        if (ros_data.axes[4] == -1):
            self.speed = 12.0 
        self.speed = round(self.speed, 3)
        if(ros_data.buttons[8] == 1):
            self.change +=1
            if(self.change == 2):
                self.change = 0
        

