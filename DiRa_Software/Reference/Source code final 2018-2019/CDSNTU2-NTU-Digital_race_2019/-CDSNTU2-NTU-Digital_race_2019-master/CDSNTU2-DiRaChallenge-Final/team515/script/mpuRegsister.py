#!/usr/bin/python2
import time
import rospy
import cv2
from std_msgs.msg import Float32
from sensor_msgs.msg import Imu

class MPU:
    def __init__(self):
        self.f = open("/home/nvidia/Data.csv", "wt")
        self.f.write("seq\tangle\tspeed\torientation.x\torientation.y\torientation.z\torientation.w\tangular_velocity.x\tangular_velocity.y\tangular_velocity.z\tlinear_acceleration.x\tlinear_acceleration.y\tlinear_acceleration.z\n")

        self.angle = None
        self.seq = 0.0
        self.speed = None
        self.orientation = None
        self.angular_velocity = None
        self.linear_acceleration = None
        self.subscriber = rospy.Subscriber("/imu_angle", Float32, self.Imu_angleCallBack, queue_size = 1)
        self.subscriber = rospy.Subscriber("/mpu_9250/imu", Imu, self.ImuCallBack, queue_size = 1)
        self.subscriber = rospy.Subscriber("/set_speed_car_api", Float32, self.SpeedCallBack, queue_size = 1)
    
    def Imu_angleCallBack(self, imu_angle):
        self.angle = imu_angle.data;
        #print (self.angle)
    
    def SpeedCallBack(self, speed):
        self.speed = speed.data;

    def ImuCallBack(self, Imu):
        self.seq = Imu.header.seq
        self.orientation = Imu.orientation
        self.angular_velocity = Imu.angular_velocity
        self.linear_acceleration = Imu.linear_acceleration

        print (str(self.seq) + "\t" + 
		str(self.angle) + "\t" +
		str(self.speed) + "\t" +
		
		str(self.orientation.x) + "\t" + 
		str(self.orientation.y) + "\t" + 
		str(self.orientation.z) + "\t" + 
		str(self.orientation.w) + "\t" + 

		str(self.angular_velocity.x) + "\t" + 
		str(self.angular_velocity.y) + "\t" + 
		str(self.angular_velocity.z) + "\t" + 

		str(self.linear_acceleration.x) + "\t" + 
		str(self.linear_acceleration.y) + "\t" + 
		str(self.linear_acceleration.z) + "\n")

        self.f.write (str(self.seq) + "\t" + 
		str(self.angle) + "\t" +
		str(self.speed) + "\t" +
		
		str(self.orientation.x) + "\t" + 
		str(self.orientation.y) + "\t" + 
		str(self.orientation.z) + "\t" + 
		str(self.orientation.w) + "\t" + 

		str(self.angular_velocity.x) + "\t" + 
		str(self.angular_velocity.y) + "\t" + 
		str(self.angular_velocity.z) + "\t" + 

		str(self.linear_acceleration.x) + "\t" + 
		str(self.linear_acceleration.y) + "\t" + 
		str(self.linear_acceleration.z) + "\n")

if __name__ == "__main__":
    rospy.init_node('MPU', anonymous=True)
    mpu = MPU()
    try:
        rospy.spin()
    except KeyboardInterrupt:
        exit
    
