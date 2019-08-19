#!/usr/bin/env python
# license removed for brevity
import rospy
import numpy as np
import time
from sensor_msgs.msg import Imu
from std_msgs.msg import String, Float32, Bool
import sys
rospy.init_node('Angle_Calibration', anonymous=True, disable_signals=True)
import config as cf

def pub_angle(angle):
    angle_publisher = rospy.Publisher('/angle', Float32, queue_size=1)
    angle_publisher.publish(angle)


pos_cam_pub = rospy.Publisher('/set_pos_camera_api', Float32, queue_size=1)
pos_cam_pub.publish(0)
cf.t = time.time()
cf.gyro_z = 0
cf.angle = 0
cf.wz = 0.0
cf.last_gyro_z = 0.0


def imu(data):
    delta_t = time.time() -cf.t
    cf.t = time.time()

    cf.gyro_z = data.angular_velocity.z
    cf.wz += delta_t*(cf.gyro_z+cf.last_gyro_z)/2.0
    cf.angle = cf.wz*180.0/np.pi
    cf.last_gyro_z =  cf.gyro_z
    pub_angle(cf.angle)


def reset(data):
	print "Reset MPU"
	cf.gyro_z = 0
	cf.angle = 0
	cf.wz = 0.0
	cf.last_gyro_z = 0.0

	cf.gyro_y = 0
	cf.angle_y = 0
	cf.wy = 0.0
	cf.last_gyro_y = 0.0

def listenner():
    rospy.Subscriber("/mpu_9250_node/imu", Imu, imu, queue_size=1)
    #rospy.Subscriber("/mpu_9250_node/reset", Float32, reset, queue_size=1)
    rospy.spin()

listenner()
    


