#!/usr/bin/env python
# license removed for brevity
import rospy
import cv2
import numpy as np
import time
import threading
from std_msgs.msg import String, Float32, Bool
from sensor_msgs.msg import CompressedImage
import sys
import config as cf
import rospkg 
rospack = rospkg.RosPack()
path = rospack.get_path('r4f')
from primesense import openni2#, nite2
from primesense import _openni2 as c_api
openni2.initialize(path+'/src/modules') #
dev = openni2.Device.open_any()
rgb_stream = dev.create_color_stream()
rgb_stream.set_video_mode(c_api.OniVideoMode(pixelFormat=c_api.OniPixelFormat.ONI_PIXEL_FORMAT_RGB888, resolutionX=640, resolutionY=480, fps=30))
rgb_stream.start()
cf.out = cv2.VideoWriter(path+'/output.avi', cv2.VideoWriter_fourcc(*'XVID'), 30.0, (480, 320))
rospy.init_node('hand_control', anonymous=True, disable_signals=True)

cf.speed = 0
cf.steer = 0
cf.min_steer = 15
cf.is_record = False
cf.pause = True
cf.running = True
cf.t = time.time()
cf.t2 =  time.time()
cf.time_point = time.time()
cf.time_point1 = time.time()

def set_pos_cam(pos_cam):
    pos_cam_pub = rospy.Publisher('/set_pos_camera_api', Float32, queue_size=1)
    pos_cam_pub.publish(pos_cam)

def set_led(led_stt):
    led_pub = rospy.Publisher('/led_status', Bool, queue_size=1)
    led_pub.publish(led_stt)

def set_speed(speed):
    if cf.pause:
	speed = 0
    if speed > 30:
	speed = 30
    if speed < -30:
	speed = 30
    speed_pub = rospy.Publisher('/set_speed_car_api', Float32, queue_size=1)
    speed_pub.publish(speed)


def set_steer(steer):
    steer_pub = rospy.Publisher('/set_steer_car_api', Float32, queue_size=1)
    steer_pub.publish(steer)


def set_lcd(text):
    lcd_pub = rospy.Publisher('/lcd_print', String, queue_size=1)
    lcd_pub.publish(text)

def print_speed_lcd():
   set_lcd("0:0:          ")
   time.sleep(0.1)
   set_lcd("0:0:Speed = "+str(cf.speed))
   time.sleep(0.1)

def get_ss_status(data):
	set_led(not data.data)
        if not data.data:
	    cf.pause = True
	    set_speed(0)
	time.sleep(0.1)


rospy.Subscriber("/ss_status", Bool, get_ss_status, queue_size=1)

def get_rgb():
    print("get_rbg started!")
    while cf.running:
        bgr   = np.fromstring(rgb_stream.read_frame().get_buffer_as_uint8(),dtype=np.uint8).reshape(480,640,3)
        rgb   = cv2.cvtColor(bgr,cv2.COLOR_BGR2RGB)
        rgb = cv2.resize(rgb,(480, 320))
        img = rgb[:, ::-1, :]
	if cf.is_record:
		cf.out.write(img)
	cv2.imshow("img", img)
	k = cv2.waitKey(1)
	if k == ord('q'):
		cf.speed = 0
		cf.steer = 0
		set_speed(cf.speed)
		set_steer(cf.steer)
		print_speed_lcd()
		cf.out.release()
		cf.running = False
		rospy.signal_shutdown("Bo may thich")
	if k == ord('w'):
		cf.pause = False
		if cf.speed == 0:
		    cf.speed = 9
		cf.speed+=1
		set_speed(cf.speed)
		print_speed_lcd()
	if k == ord('s'):
		if cf.speed == 0:
			cf.speed = -5
		if cf.speed>-15:
			cf.speed-=1
		set_speed(cf.speed)
		print_speed_lcd()
	if k == ord('a'): #55
		if cf.steer==0:
			cf.time_point1 = time.time()
		if cf.steer < cf.min_steer:
			cf.steer = cf.min_steer
		elif cf.steer <60:   
			cf.steer += 3
			
		cf.time_point = time.time()
		set_steer(cf.steer)
	if k == ord('d'):
		if cf.steer==0:
			cf.time_point1 = time.time()
		if cf.steer > -cf.min_steer:
			cf.steer = -cf.min_steer
		elif cf.steer >-60: 
			cf.steer -= 3
		cf.time_point = time.time()
		set_steer(cf.steer)

	if k ==32:
		cf.speed = 0
		set_speed(cf.speed)
	if k==ord('v'):
		cf.is_record = True
		set_lcd("0:1:Rec = " + str(cf.is_record)+"      ")
	if k==ord('b'):
		cf.is_record = False
		set_lcd("0:1:Rec = " + str(cf.is_record)+"      ")
	if k == -1:
		if time.time() - cf.time_point1 >0.5:
			if time.time() - cf.time_point >0.01:
				cf.steer -= 3*np.sign(cf.steer)
				set_steer(cf.steer)
    print("get_rbg stoped")
set_pos_cam(0)
time.sleep(0.2)
print_speed_lcd()
time.sleep(0.2)
set_lcd("0:1:Rec = " + str(cf.is_record)+"      ")
get_rbg_thread = threading.Thread(name = "get_rbg_thread", target=get_rgb)
get_rbg_thread.start()

rospy.Subscriber("/ss_status", Bool, get_ss_status, queue_size=1)

