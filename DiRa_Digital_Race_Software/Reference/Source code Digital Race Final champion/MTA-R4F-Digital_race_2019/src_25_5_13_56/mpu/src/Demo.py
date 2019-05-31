#!/usr/bin/env python
# license removed for brevity
import rospy
import cv2
import numpy as np
import time
import threading
from std_msgs.msg import String, Float32, Bool
from sensor_msgs.msg import CompressedImage, Imu
import sys
import config as cf
rospy.init_node('Demo', anonymous=True, disable_signals=True)

cf.t1 = time.time()
cf.t2 = time.time()
cf.fps1 = 0
cf.fps2 = 0
cf.angle = 0
cf.imu_data = None
cf.running  = True
def listenner():
    rospy.Subscriber("/mpu_9250_node/imu", Imu, imu, queue_size=1)
    rospy.spin()

cf.clear = False    
def imu(data):
    cf.fps2 = 1/(time.time()-cf.t2)
    cf.t2 = time.time()
    cf.imu_data = data
    cf.angle = data.angular_velocity.z
    cf.clear = True
import imutils
def draw_car(img, x, y, angle):
    x+=300
    y+=300
    if x>579:
	x = 579
    if x<20:
	x = 20
    if y>579:
	y = 579
    if y<20:
	y = 20
    HEIGHT = 40
    WIDTH = 40
    car2 = cv2.imread("1.png", 1)
    car = car2.copy()
    car[car2>100]=0
    car[car2<=100]=255
    car = cv2.resize(car, (WIDTH, HEIGHT))
    car = imutils.rotate(car, angle)
    img[x-HEIGHT//2:x+HEIGHT//2, y-WIDTH//2:y+WIDTH//2, :] = car
    return img

def get_angle_LUT():
    x1 = np.array([0, 45, 90, 135, 180, 225,  270, 315, 359])
    x2 = np.array([0, 96, 128, 159, 188, 212, 243, 280, 359])
    x = []
    for i in range(x1.shape[0]-1):
        xi = (np.linspace(x2[i], x2[i+1]-1, x2[i+1]- x2[i])-x2[i])/(x2[i+1]-x2[i])*(x1[i+1] - x1[i])+x1[i]
        for xii in xi:
	        x.append(np.round(xii,2))
    return x

angle_LUT = get_angle_LUT()
cf.ax = 0
cf.ay = 0
cf.vx = 0
cf.vy = 0
cf.fps = 0
def SensorFusion():
    time.sleep(1)
    print("SensorFusion thread started!")
    last_position = np.array([0, 0])
    new_position = last_position.copy()
    ax_old = 0
    ay_old = 0
    sleep_time = 1
    t = time.time()
    t_fps = time.time()-0.1
    g = 9.8066
    while cf.running:
        if cf.clear:
                angle_x = cf.imu_data.angular_velocity.x
                angle_y = cf.imu_data.angular_velocity.y
	        ax_new = cf.imu_data.linear_acceleration.x + g*np.sin(cf.imu_data.angular_velocity.y)
	        ay_new = cf.imu_data.linear_acceleration.y - g*np.sin(cf.imu_data.angular_velocity.x)
	        delta_t = time.time()-t
	        t = time.time()
                cf.ax = (ax_new+ax_old)/2
                cf.ay = (ay_new+ay_old)/2
	        ax_old = ax_new
	        ay_old = ay_new
	        cf.vx += cf.ax*delta_t
	        cf.vy += cf.ay*delta_t
                cf.fps = np.round(1/(time.time()-t_fps), 2)
                t_fps = time.time()
                cf.clear = False
	'''angle_offset = -angle + 297
	vx_offset = vx*np.cos(np.pi*angle_offset/180.0)
	vy_offset = vx*np.sin(np.pi*angle_offset/180.0)
	y += vx_offset*delta_t
	x -= vy_offset*delta_t'''
	
def Show():
    time.sleep(2)
    print("Show thread started!")
    sleep_time = 1
    white = (255, 255, 255)
    font = cv2.FONT_HERSHEY_SIMPLEX
    graph = np.zeros((600, 600 ), np.uint8)
    graph[200,:] = 255
    graph[400,:] = 255
    last_position = np.array([0, 0])
    new_position = last_position.copy()
    while cf.running:
	graph[60:-1,0:-5] = graph[60:-1, 4:-1]
        graph[0:60,:] = 0
	graph[int(-cf.vx*100)-2+200:int(-cf.vx*100)+2+200, 500:504] = 255
	graph[int(-cf.ax*100)-2+400:int(-cf.ax*100)+2+400, 500:504] = 255
        cv2.putText(graph, "fps: "+str(cf.fps), (30, 50), font, 0.5, white, 1)
        cv2.putText(graph, "fps2: "+str(cf.fps2), (200, 50), font, 0.5, white, 1)
	
	cv2.imshow("graph", graph)
        img = np.zeros((600, 600, 3 ), np.uint8)
        angle = angle_LUT[int(cf.angle)-1]
        angle_offset = -angle -75
        img = draw_car(img, new_position[0], new_position[1], angle_offset)
	cv2.putText(img, "cf.angle: "+str(int(cf.angle)), (30, 50), font, 0.5, white, 1)
        cv2.putText(img, "angle: "+str(int(angle)), (200, 50), font, 0.5, white, 1)
        cv2.imshow("img", img)
        k = cv2.waitKey(sleep_time)
        if k == ord('q'):
            cf.running = False
            print("Show thread stoped!")
            rospy.signal_shutdown("Bo may thich")
            break
	if k == 32:
		sleep_time = 1- sleep_time
        if k == ord('g'): 
	    cf.vx = 0
	    cf.vy = 0

SensorFusion_thread = threading.Thread(name= 'SensorFusion', target = SensorFusion)
SensorFusion_thread.start()        
show_thread = threading.Thread(name= 'Show', target = Show)
show_thread.start()
listenner()
'''
def Show():
    time.sleep(2)
    print("Show thread started!")
    last_position = np.array([0, 0])
    new_position = last_position.copy()
    ax_old = 0
    ay_old = 0
    x = 0
    y = 0
    vx = 0
    vy = 0
    angle = 0
    lines = np.zeros((600, 600, 3 ), np.uint8)
    sleep_time = 1
    cf.t = time.time()
    white = (255, 255, 255)
    font = cv2.FONT_HERSHEY_SIMPLEX
    graph = np.zeros((600, 600 ), np.uint8)
    graph[200,:] = 255
    graph[400,:] = 255
    while cf.running:
        img = np.zeros((600, 600, 3 ), np.uint8)
        angle_x = cf.imu_data.angular_velocity.x
        angle_y = cf.imu_data.angular_velocity.y
	ax_new = cf.imu_data.linear_acceleration.x + 9.8*np.sin(cf.imu_data.angular_velocity.y)
	ay_new = cf.imu_data.linear_acceleration.y - 9.8*np.sin(cf.imu_data.angular_velocity.x)
	delta_t = time.time()-cf.t
	cf.t = time.time()
        ax = (ax_new+ax_old)/2
        ay = (ay_new+ay_old)/2
	ax_old = ax_new
	ay_old = ay_new
	vx += ax*delta_t
	vy += ay*delta_t
	if vx<0.01:
		vx = 0
	if abs(vy)<0.01:
		vy = 0

	angle_offset = -angle + 297
	vx_offset = vx*np.cos(np.pi*angle_offset/180.0)
	vy_offset = vx*np.sin(np.pi*angle_offset/180.0)
	y += vx_offset*delta_t
	x -= vy_offset*delta_t
	graph[:,0:-5] = graph[:, 4:-1]
	graph[int(-vx*100)-2+200:int(-vx*100)+2+200, 500:504] = 255
	graph[int(-ax_new*100)-2+400:int(-ax_new*100)+2+400, 500:504] = 255
	cv2.imshow("graph", graph)
	new_position = (np.array([-x, -y])*100).astype(np.int16)
	img = draw_car(img, new_position[0], new_position[1], angle_offset)
	cv2.putText(img, "fps: "+str(np.round(1/delta_t,2)), (50, 50), font, 0.5, white, 1)
	cv2.putText(img, "angle: "+str(np.round(angle,2)), (50, 80), font, 0.5, white, 1)
        cv2.putText(img, "angle_offset: "+str(np.round(angle_offset,2)), (50, 110), font, 0.5, white, 1)
	cv2.putText(img, "ax: "+str(np.round(ax,5)), (50, 140), font, 0.5, white, 1)
	cv2.putText(img, "ay: "+str(np.round(ay,5)), (50, 170), font, 0.5, white, 1)
	cv2.putText(img, "vx: "+str(np.round(vx,5)), (50, 200), font, 0.5, white, 1)
	cv2.putText(img, "vy: "+str(np.round(vy,5)), (50, 230), font, 0.5, white, 1)
        lines= cv2.line(lines, (last_position[1]+300,last_position[0]+300) , (new_position[1]+300, new_position[0]+300), (255, 255, 255), 1)
        img[lines >200] = 255
        cv2.imshow("img", img)
        #cv2.imshow("lines", lines)
        k = cv2.waitKey(sleep_time)
        last_position[0]= new_position[0]
        last_position[1] = new_position[1]
        angle = angle_LUT[int(cf.angle)-1]
        if k == ord('q'):
            print("Show thread stoped!")
            rospy.signal_shutdown("Bo may thich")
            break
	if k == 32:
		sleep_time = 1- sleep_time
        if k == ord('g'):  
            lines = np.zeros((600, 600, 3 ), np.uint8)
	    x = 0
	    y = 0
	    vx = 0
	    vy = 0
            last_position = np.array([0, 0])
            new_position = last_position.copy()
'''
