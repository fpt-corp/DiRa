import config as cf
import numpy as np
import time
import cv2 
import imutils
import threading
from PyCallCpp import get_shortest_route


cf.Guild_Line_LUT = [{

    ########################################
    "3L5L":[(-1,60,'c1'),(-1,10),(-1,0,'c1')], #
    "3L8L":[(-1,60,'c1'),(1,180),(-1,0,'c1')], #ok

    "4L5L":[(1,60,'c1'),(1,10),(1,0,'c1')], #ok
    "4L7L":[(1,60,'c1'),(-1,180),(1,0,'c1')], #ok

    "5L9R":[(-1,45,'s20','d30'),(1,0,'d60'),(-1,60,'d30','s20'),(-1,70,'d60','s20'), (1,155,'s20'),(-1,173,'s20')], #ok
   
    "7R3L":[(1,5,'d30','t3.5'),(-1,180),(-1,135,'l1'),(-1,180,'c1')], #ok
    "7R4L":[(1,5,'d30','t3.5'),(-1,125),(1,130),(1,100),(1,180,'c1')], #ok
    "7R5L":[(1,5,'d30','t3.5'),(-1,160),(-1,180,'t6','c1')],  #ok
    "7L9L":[(-1,90,'s21','c1'),(-1,90,'t1','s19','c1'),(-1,10,'s13','t0.5','l2'),(1,10,'t0.2')], #Adv #ok
  
    "8R3L":[(-1,30,'d30'),(-1,70,'c1'),(1,145),(-1,125),(-1,180,'c1')], #ok
    "8R4L":[(-1,30,'d30'),(-1,70,'c1'),(-1,70,'d80'),(1,180),(1,135),(1,180,'c1')], #ok
    "8R5L":[(-1,30,'d30'),(-1,70,'c1'),(-1,70,'d80'),(1,140),(1,180,'t5.5','c1')], #ok
    "8L9L":[(1,13)],
  
    "9R7R":[(1,35,'l2','s21'),(1,90,'s21','c1'),(1,90,'t1.7','s19','c1'),(1,10,'s14','t0.5','l2'),(1,10,'t0.2')], #ok
    "9R8R":[(-1,13,'t4', 's24', 'd30')], #ok
    "9L10":[(1, 13, 'd60'), (-1, 70, 'd30'),  (1, 170, 't0.8','s17', 'd30')], #vat can
    "5L5E":[(1,0,'t1','s18'),(1,280,'s18','d15','l1', 'p0')], #ok
},


{
    
  
}
]

cf.finish = False
def navigator():
    print "Navigator thread is started!"
    while cf.running:
	time.sleep(0.01)
	while cf.finish:
		time.sleep(0.1)
	cf.go = True
	print("Let 's run !")
	#cf.shortest_route = get_shortest_route(cf.route)
	cf.shortest_route = ["7L", "9L"]
	#96745
	#cf.shortest_route = ["5L", "9R","8R", "5L", "9R","7R", "4L", "5L", "5E"]
	#96735
	#cf.shortest_route = ["5L", "9R","8R", "5L", "9R","7R", "3L", "5L", "5E"]
	#96835
	#cf.shortest_route = ["5L", "9R","8R", "5L", "9R","8R", "3L", "5L", "5E"]
	#96845
	#cf.shortest_route = ["5L", "9R","8R", "5L", "9R","8R", "4L", "5L", "5E"]
	#27185
	#cf.shortest_route = ["5L", "9R","7R", "5L", "9R","8R", "5L", "5E"]
	#173810
	#cf.shortest_route = ["5L", "9R","7R", "3L", "8L","7L", "10", "10E"]
	#284710
	#cf.shortest_route = ["5L", "9R","8R", "4L", "7L","9L", "10", "10E"]
	print(cf.shortest_route)
	cf.angle_checkpoint = cf.angle
        i=0
        while i < len(cf.shortest_route)-1:
	    time.sleep(0.01)
	    if cf.shortest_route[i][0:2] == '12' or cf.shortest_route[i][0:2] =='21':
		i+=1 
            this_path = cf.shortest_route[i]+cf.shortest_route[i+1]
	    if cf.shortest_route[i+1][0:2] == '12' or cf.shortest_route[i+1][0:2] =='21':
		this_path+=cf.shortest_route[i+2]
            stask = cf.Guild_Line_LUT[cf.map].get(this_path)
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


