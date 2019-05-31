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
from modules.Control import PID, Fuzzy
from modules.Advance import Advance_lane, Speed_prediction
from modules.Route2 import navigator
from modules.PyCallCpp import get_shortest_route
from modules.Classic import Classic, Pre
from modules.Camera import get_rgb, get_depth
import config as cf
import rospkg 
rospack = rospkg.RosPack()
path = rospack.get_path('r4f')
rospy.init_node('run', anonymous=True, disable_signals=True)
cf.pos_cam_pub = rospy.Publisher('/set_pos_camera_api', Float32, queue_size=1)

def set_led(led_stt):
    led_pub = rospy.Publisher('/led_status', Bool, queue_size=1)
    led_pub.publish(led_stt)

def reset_mpu():
	reset_mpu = rospy.Publisher("/mpu_9250_node/reset", Float32, queue_size=1)
	reset_mpu.publish(1.0)

def set_speed(speed):
    if cf.pause or cf.pause2:
	speed = 0
    if cf.barrier is not None and speed >10:
        speed -=5
    if speed > 30:
	speed = 30
    if speed < -30:
	speed = -30
    speed_pub = rospy.Publisher('/set_speed_car_api', Float32, queue_size=1)
    speed_pub.publish(speed)


def set_steer(steer):
    steer_pub = rospy.Publisher('/set_steer_car_api', Float32, queue_size=1)
    steer_pub.publish(steer)


def set_lcd(text):
    lcd_pub = rospy.Publisher('/lcd_print', String, queue_size=1)
    lcd_pub.publish(text)


def print_lcd(line):
    texts = ["00:0:Speed=", "00:1:Max=", "00:2:Mode=", "00:3:Rec=", "10:0:Angle="]
    lane_modes = ["Cla", "Adv", "Line"]
    info = [str(cf.speed_offset), str(cf.max_addition_speed), lane_modes[cf.lane_mode], str(cf.is_record), str(int(cf.angle))]
    text = texts[line]+info[line];
    space = (14 - len(text))*" "
    text +=space
    set_lcd(text)


def get_ss_status(data):
	set_led(not data.data)
        if data.data is True:
            time.sleep(1)
	    cf.pause2 = not data.data
        else:
            cf.pause2 = not data.data
	time.sleep(0.1)

def get_bt1_status(data):  #ngoai cung ben phai
    if data.data and not cf.bt1_old:
	cf.pause = not cf.pause
    cf.bt1_old = data.data  
    time.sleep(0.1) 

def get_bt2_status(data):
    if data.data and not cf.bt2_old:
	if cf.cursor == 0:
	    cf.speed_offset -= 1
	if cf.cursor == 1:
	    cf.max_addition_speed -= 1
        print_lcd(cf.cursor)
    cf.bt2_old = data.data
    time.sleep(0.1)

def get_bt3_status(data):
    if data.data and not cf.bt3_old:
	if cf.cursor == 0:
	    cf.speed_offset += 1
	    cf.pause = False
	if cf.cursor == 1:
	    cf.max_addition_speed += 1
	if cf.cursor == 2:
	    cf.lane_mode = (cf.lane_mode+1)%3
	if cf.cursor == 3:
	    cf.is_record = not cf.is_record
	if cf.cursor == 4:
	    cf.lane_mode = 1
	    time.sleep(0.5)
	    cf.angle_offset = - cf.angle0
	    cf.angle_y_offset = - cf.angle1+180
	    cf.angle_y_array = cf.angle_y_array*0
	    cf.finish = False
	    cf.go = False
	print_lcd(cf.cursor)
	    
    cf.bt3_old = data.data
    time.sleep(0.1)


def get_bt4_status(data):
    if data.data and not cf.bt4_old:
	cf.cursor = (cf.cursor+1)%5
        print_lcd(cf.cursor)
    cf.bt4_old = data.data
    time.sleep(0.1)

cf.angle_offset = 0
cf.angle_y_offset = 0

def get_angle(data):
    cf.angle0 = data.data 
    cf.angle = cf.angle0 + cf.angle_offset 
    if cf.angle<0:
	cf.angle = 360 +cf.angle

def get_angle_y(data):
    cf.angle1 = data.data 
    cf.angle_y = cf.angle1 + cf.angle_y_offset 
    if cf.angle_y>360:
	cf.angle_y =cf.angle_y- 360
    lech = cf.angle_y - cf.angle_y_checkpoint
    if lech>180:
	lech = 360 - lech
    cf.angle_y_array = np.concatenate([cf.angle_y_array, [lech]])
    cf.angle_y_array = cf.angle_y_array[1:]
cf.route =''
cf.old_route =''
def get_route_massage(data):
	route_message = data.data
	route_arr = route_message.split('-')
	num = str(len(route_arr))
	cf.route = num
	for checkoint in route_arr:
		cf.route = cf.route + " "+str(checkoint)
	print "#"*20
	print "Received new route message:", cf.route
	if cf.route != cf.old_route:
		cf.shortest_route = get_shortest_route(cf.route)
		cf.lane_mode = 1
		time.sleep(0.5)
		cf.angle_offset = - cf.angle0
		cf.angle_y_offset = - cf.angle1+180
		cf.angle_y_array = cf.angle_y_array*0
		cf.finish = False
		cf.go = False
		cf.old_route = cf.route
	else:
		print "This is old route Message"

def angle_control():
    if cf.control_mode == 0:
    	angle = PID(cf.center- 240)
    if cf.control_mode ==1:
	angle = Fuzzy(cf.center - 240)
    set_steer(angle)

def speed_control():
    if cf.speed_mode == 0:
	cf.speed = int(cf.speed_offset + cf.addition_speed_ratio*cf.max_addition_speed)
    if cf.speed_mode == 1:
	cf.speed = cf.dynamic_speed
    set_speed(cf.speed)


# System Variables
cf.bt1_old =cf.bt2_old = cf.bt3_old = cf.bt4_old = False
cf.cursor = 0
cf.HEIGHT = 320
cf.WIDTH = 480
cf.is_record = False
cf.out = cv2.VideoWriter(path+'/output.avi', cv2.VideoWriter_fourcc(*'XVID'), 30.0, (cf.WIDTH, cf.HEIGHT))
cf.img = np.zeros((480, 320, 3), np.uint8)
cf.depth =np.zeros((240, 320), np.uint8)
cf.img_show = np.zeros((320, 480), np.uint8)
cf.img_show2 = np.zeros((320, 480), np.uint8)
cf.running = True
cf.pause = True
cf.pause2 = True
cf.syn = True

# Speed Variables
cf.speed = 0
cf.speed_mode = 0
cf.speed_offset = 18
cf.max_addition_speed = 2
cf.addition_speed_ratio = 0
cf.dynamic_speed = 0


#Lane  Variables
cf.map = 0
cf.steer = 0
cf.center = 240
cf.side_tracking = 0
cf.control_mode = 1
cf.lane_mode = 0
cf.time_side = time.time()
cf.pos_cam_default = 20
cf.barrier = None
cf.time_barrier = time.time()
cf.distance_default = 55
cf.distance = cf.distance_default
cf.control_mode = 0
cf.lane_choice = 0
cf.x_distance_default = 70
cf.x_distance = 0
#cf.center_old = 0

# Navigator Global Variables
cf.finish = False
cf.angle = 0
cf.angle_y = 0
cf.angle1 = 0
cf.angle_checkpoint = cf.angle
cf.angle_y_checkpoint = 0
cf.time_bridge = time.time() - 100
cf.angle_y_array = np.zeros(10)

def get_center(img_):
    img = img_.copy()
    if cf.side_tracking == -1:
        cv2.rectangle(img, (0, 0), (80, 80), (0, 0, 255), -1)
    elif cf.side_tracking == 1:
        cv2.rectangle(img, (400, 0), (480, 80), (0, 0, 255), -1)
    else:
	cv2.rectangle(img, (200, 0), (280, 80), (0, 0, 255), -1)
    if cf.lane_mode == 1:
	center = Advance_lane(img)
	cf.center = center
	cv2.circle(img, (cf.center, 250), 5, (0, 255, 0), -1)
	cf.img_show = img
    if cf.lane_mode == 0:	
	side_tracking = cf.side_tracking
        if side_tracking == -1:
		side_tracking = 0	
	img, img2, center= Classic(img, side_tracking, cf.lane_choice, cf.distance, cf.center, cf.x_distance)
	if time.time()-cf.time_side < 0.075:
		cf.center = 240 - 30*(1-2*side_tracking)
		#cf.center_old = cf.center
	else:
		cf.center = center
        cf.img_show = img
	cf.img_show2 = img2


def speed_Prediction():
    print("Speed_Prediction started!")
    while cf.running:
        cf.addition_speed_ratio = Speed_prediction(cf.img)
    print("Speed_Prediction stoped!")
cf.time_fps = time.time()-1
cf.fps = 0
def main():
    print("Main started!")
    time.sleep(1)
    set_lcd("0:0:                    ");time.sleep(0.2)
    print_lcd(0);time.sleep(0.2)
    print_lcd(1);time.sleep(0.2)
    print_lcd(2);time.sleep(0.2)
    print_lcd(3);time.sleep(0.2)
    print_lcd(4);
    while cf.running:
        if cf.syn:
	    cf.fps = int(1/(time.time()-cf.time_fps))
	    cf.time_fps = time.time()
	    cf.syn = False
    	    get_center(cf.img)
            angle_control()
	    speed_control()
time_lcd = time.time()
def show():
    global t_point_fps, time_lcd
    print("Show_thread started !")
    while cf.running:     
	    font = cv2.FONT_HERSHEY_SIMPLEX
	    control_modes = ["Fuz", "PID"]
	    lane_modes = ["Cla", "Adv", "Line"]
	    img = cf.img_show.copy()
	    if cf.is_record:
		cf.out.write(cf.img)
	    if cf.barrier == 0:
		cv2.rectangle(img, (0, 80), (80, 160), (255, 0, 0), -1)
	    if cf.barrier == 1:
		cv2.rectangle(img, (400, 80), (480, 160), (255, 0, 0), -1)
	    cv2.putText(img,"S: "+str(cf.speed),(90,25), font, 0.8, (255,0,0), 2, 1)
	    cv2.putText(img,str(int(cf.angle)),(350,25), font, 0.8, (255,0,0), 2, 1)
	    cv2.putText(img,str(int(cf.angle_checkpoint)),(250,25), font, 1, (255,0,0), 2, 1)
	    cv2.putText(img,str(cf.distance),(200,50), font, 0.8, (255,0,0), 2, 1)
            cv2.putText(img,str(cf.pos_cam_default),(90,50), font, 0.8, (255,0,0), 2, 1)
	    cv2.putText(img,str(cf.lane_choice),(300,50), font, 0.8, (255,0,0), 2, 1)
	    cv2.putText(img,"REC: "+str(cf.is_record)+"   "+ str(cf.fps),(90,75), font, 0.8, (255,0,0), 2, 1)
	    cv2.imshow("img", img)
	    #cv2.imshow("img2", cf.img_show2)
	    #cv2.imshow("cf.depth", cf.depth)
	    k = cv2.waitKey(30)
	    if k == ord('u'):
		cf.distance_default +=5
	    if k == ord('i'):
		cf.distance_default -=5
	    if k == ord('t'):
		cf.max_addition_speed +=1
		print_lcd(1)
	    if k == ord('g'):
		cf.max_addition_speed -=1
		print_lcd(1)
	    if k == ord('h'):
		cf.pos_cam_default +=2
		cf.pos_cam_pub.publish(-cf.pos_cam_default*cf.side_tracking)
	    if k == ord('j'):
		cf.pos_cam_default -=2
		cf.pos_cam_pub.publish(-cf.pos_cam_default*cf.side_tracking)
	    if k == ord('w'):
		cf.speed_offset += 1
		#cf.pause = False
		print_lcd(0)
		print (cf.speed)
	    if k == ord('s'):
		cf.speed_offset -= 1
		print_lcd(0)
		print (cf.speed)
	    if k == 32:
		cf.pause = True
	    if k == ord('p'):
		cf.pause = False
	    if k == ord('m'):
		print "#"*20
		cf.map = 1-cf.map
		cf.lane_mode = 1
		time.sleep(0.5)
		cf.angle_offset = - cf.angle0
		cf.angle_y_offset = - cf.angle1+180
		cf.angle_y_array = cf.angle_y_array*0
		cf.finish = False
		cf.go = False
	    if k == ord('r'):
		cf.side_tracking += 1
		if cf.side_tracking==2:
			cf.side_tracking = -1
		cf.time_side = time.time()
		if cf.lane_mode == 0:
			cf.pos_cam_pub.publish(-cf.pos_cam_default*cf.side_tracking)
		else:
			cf.pos_cam_pub.publish(0)
	    if k == ord('f'):
		print "#"*20
		reset_mpu()
		time.sleep(0.5)
		cf.lane_mode = 1
		time.sleep(0.5)
		cf.angle_offset = - cf.angle0
		cf.angle_y_offset = - cf.angle1+180
		cf.angle_y_array = cf.angle_y_array*0
		cf.finish = False
		cf.go = False
	    if k == ord('e'):
		cf.control_mode = 1-cf.control_mode
		print(cf.control_mode)
	    if k == ord('z'):
		cf.lane_choice = (1+cf.lane_choice)%3
		print(cf.lane_choice)
	    if k == ord('c'):
		cf.lane_mode = 1-cf.lane_mode
		if cf.lane_mode == 0:
			cf.pos_cam_pub.publish(-cf.pos_cam_default*cf.side_tracking)
		else:
			cf.pos_cam_pub.publish(0)
		print_lcd(2)
	    if k == ord('v'):
		cf.is_record = not cf.is_record
		print_lcd(3)
	    if k == ord('q'):
		cf.speed = 0
		cf.steer = 0
		cf.finish = False
		cf.running = False
		cf.go = False
		set_speed(cf.speed)
		set_steer(cf.steer)
		cf.pos_cam_pub.publish(0)
		cf.out.release()
		rospy.signal_shutdown("Bo may thich")
	    if time.time()-time_lcd > 1:
		print_lcd(4)
		time_lcd = time.time()


def listenner():
    rospy.Subscriber("/ss_status", Bool, get_ss_status, queue_size=1)
    rospy.Subscriber("/bt1_status", Bool, get_bt1_status, queue_size=1)
    rospy.Subscriber("/bt2_status", Bool, get_bt2_status, queue_size=1)
    rospy.Subscriber("/bt3_status", Bool, get_bt3_status, queue_size=1)
    rospy.Subscriber("/bt4_status", Bool, get_bt4_status, queue_size=1)
    rospy.Subscriber("/angle", Float32, get_angle, queue_size=1)
    rospy.Subscriber("/angle_y", Float32, get_angle_y, queue_size=1)
    rospy.Subscriber("/route", String, get_route_massage, queue_size=1)
    rospy.spin()

get_rbg_thread = threading.Thread(name = "get_rbg_thread", target=get_rgb)
get_rbg_thread.start()
time.sleep(0.5)
get_depth_thread = threading.Thread(name = "get_depth_thread", target=get_depth)
#get_depth_thread.start()
main_thread = threading.Thread(name= "main_thread", target= main)
main_thread.start()
speed_thread = threading.Thread(name= "speed_Prediction", target= speed_Prediction)
speed_thread.start()
show_thread = threading.Thread(name= "show information", target= show)
show_thread.start()
navigator_thread = threading.Thread(name= "Navigator_thread", target = navigator)
navigator_thread.start()
listenner()
