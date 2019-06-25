#!/usr/bin/python2

import rospy
from std_msgs.msg import Float32
from std_msgs.msg import String
from sensor_msgs.msg import CompressedImage
import cv2
import numpy as np
import sys
from cv_bridge import CvBridge, CvBridgeError
import time
from car_control import *
from joy_conrtrol import *
from hal_control import *
  
def main(args):
	rospy.init_node('camera_depth_input_node', anonymous=True)
	joy = JC()
	hal = Hal()
	#config = tf.ConfigProto()
	#config.gpu_options.allow_growth = True
	car = Car(joy, hal)
	#car.auto_control()
        #car.joy_control()
	car.control()
		
	try:
		rospy.spin()
	except KeyboardInterrupt:
		print("Shutting down ROS Image feature detector module")
	cv2.destroyAllWindows()
	
if __name__ == '__main__':
    main(sys.argv)
