import config as cf
import numpy as np
import time
import cv2 
import imutils
import threading
from PyCallCpp import get_shortest_route
cf.vatcan12 = 30
cf.vatcan21 = 180 - cf.vatcan12
cf.Guild_Line_LUT = {

    "5L125R":[(1,45,'s21','d30'),(1,0,'s21', 'd30'),(1, 10,'d30','s21'),(-1,60,'s21','d5'),(1,180,'s21'),(1,135,'s23'),(1,180,'l3','s24')],
    "5L215R":[(-1,45,'s21','d30'),(-1,0,'s21', 'd30'),(-1,60,'d30','s21'),(-1,70,'d60','s21'), (1,160,'s21'),(-1,135,'s23'),(-1,180,'l3','s24')],
    "5R3R":[(1,45),(1,0,'c1', 's22')],
    "3R4L":[(1, 80, 'c1'),(1, 180, 'c1', 's23')],	
    "4L5L":[(1,45,'c1'),(1,0,'s20')],
    "5L5E":[(1,0,'t1','s18'),(1,280,'s18','d15','l1', 'p0')],
}
cf.finish = False
def navigator():
    print "Navigator thread is started!"
    cf.shortest_route = ["5L","21", "5R", "3R", "4L", "5L", "5E" ]
    while cf.running:
	time.sleep(0.01)
	while cf.finish:
		time.sleep(0.1)
	cf.go = True
	print("Let 's run !")
	print "Route Message:", cf.route
	print(cf.shortest_route)
	cf.angle_checkpoint = cf.angle
        i=0
        while i < len(cf.shortest_route)-1:
	    time.sleep(0.01)
	    if cf.shortest_route[i][0] == '1' or cf.shortest_route[i][0] =='2':
		i+=1 
            this_path = cf.shortest_route[i]+cf.shortest_route[i+1]
	    if cf.shortest_route[i+1][0] == '1' or cf.shortest_route[i+1][0] =='2':
		this_path+=cf.shortest_route[i+2]
            stask = cf.Guild_Line_LUT.get(this_path)
            print("mini path:", this_path, stask)
            for j in range(len(stask)):
		print(stask[j])
		time.sleep(0.03)
		side_tracking = stask[j][0]
		lane_mode = 0
		time_sleep = 0
		speed_mode = 0
		dynamic_speed = 0
		distance = cf.distance_default
		lane_choice = 0
		x_distance = cf.x_distance_default
		parking = False
                if len(stask[j]) >2:
		    for code in stask[j][2:]:
			mode = code[0]
			value = float(code[1:])
			if mode == 't':
				time_sleep = value
			if mode == 'c':
				lane_mode =  int(value)
			if mode == 'd':
				distance = int(value)
			if mode == 's':
				dynamic_speed = int(value)
				speed_mode = 1
			if mode == 'l':
				lane_choice = int(value)
			if mode == 'x':
				x_distance = int(value)
			if mode == 'p':
				parking = True
		if side_tracking != cf.side_tracking:
			cf.time_side = time.time()
		cf.side_tracking = side_tracking
		cf.distance = distance
		if lane_mode == 0 :
			cf.pos_cam_pub.publish(-cf.pos_cam_default*cf.side_tracking)
		else:
			cf.pos_cam_pub.publish(0)
		cf.parking = parking
		cf.dynamic_speed = dynamic_speed
		cf.speed_mode= speed_mode
		cf.lane_mode = lane_mode
		cf.lane_choice = lane_choice
		cf.x_distance =x_distance	
		while cf.go:
		       x = abs(cf.angle - cf.angle_checkpoint)
		       if x>180:
		           x = 360 - x
		       if abs(x - stask[j][1])< 10:
		           break
		       time.sleep(0.01)
		time.sleep(time_sleep)
		if not cf.go:
		        break
            if not cf.go:
                break
            time.sleep(0.35)
	    i+=1
            cf.angle_checkpoint = cf.angle
    print "Navigator thread is stoped!"







        



