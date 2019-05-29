#!/usr/bin/python2
import time
import rospy
import paho.mqtt as mqtt
from std_msgs.msg import String

def callback(data):
    print(data.data)

def listener():
    rospy.init_node('subcriber')
    rospy.Subscriber('route', String, callback)
    rospy.spin()
if __name__ == '__main__':
    #client = mqtt.Client()
    #client.connect("localhost")
    #client.loop_start()
    listener()
    #client.loop_stop()
