import cv2
import numpy as np
import rospkg 
from primesense import openni2#, nite2
from primesense import _openni2 as c_api
import config as cf
import time

rospack = rospkg.RosPack()
path = rospack.get_path('r4f')
openni2.initialize(path+'/src/modules') #
dev = openni2.Device.open_any()
rgb_stream = dev.create_color_stream()
rgb_stream.set_video_mode(c_api.OniVideoMode(pixelFormat=c_api.OniPixelFormat.ONI_PIXEL_FORMAT_RGB888, resolutionX=640, resolutionY=480, fps=30))
rgb_stream.start()
depth_stream = dev.create_depth_stream()
depth_stream.set_video_mode(c_api.OniVideoMode(pixelFormat = c_api.OniPixelFormat.ONI_PIXEL_FORMAT_DEPTH_100_UM, resolutionX = 320, resolutionY = 240, fps = 30))
depth_stream.start()

def get_rgb():
    print("Get_rbg started!")
    while cf.running:
        bgr   = np.fromstring(rgb_stream.read_frame().get_buffer_as_uint8(),dtype=np.uint8).reshape(480,640,3)
        rgb   = cv2.cvtColor(bgr,cv2.COLOR_BGR2RGB)
        rgb = cv2.resize(rgb,(480, 320))
        cf.img = rgb[:, ::-1, :]
        cf.syn = True
    print("Get_rbg stoped")

def get_depth():
    print("Get_depth started!")
    while cf.running:
	while not cf.parking and cf.running:
		time.sleep(0.05)
	frame = depth_stream.read_frame()
        frame_data = frame.get_buffer_as_uint16()
        img = np.frombuffer(frame_data, dtype=np.uint16)
        img.shape = (240, 320)
        img = cv2.flip(img,1)
        #cv2.imshow("img",img)
        img2 = img.copy()
        img2 = (img2*1.0/2**8).astype(np.uint8)
        img2 = 255 - img2
	cv2.imwrite("depth.jpg", img2)
        adap = cv2.adaptiveThreshold(img2, 255,cv2.ADAPTIVE_THRESH_MEAN_C,cv2.THRESH_BINARY,51, -5)
        cut = 90
        adap = adap[cut:240, :]
        #black = np.zeros((120,320),np.uint8)
        __, contours,_ = cv2.findContours(adap,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
	barrier = None
        stt = 0
        dis =50
        center = int(cf.center*160/240) + 10*np.sign(cf.center-160)
        for cnt in contours:
            x,y,w,h = cv2.boundingRect(cnt)
            if w>40 and h>30 :
		if x+w/2>center and x<center+dis:
		    barrier = 1
		    stt+=1
		if x+w/2<center and x+w>center-dis:
		    barrier = 0
                    stt+=1
	        cv2.rectangle(img2,(x,y+cut),(x+w,y+h+cut),(255,255,255),2)
                #cv2.drawContours(img2, [cnt], -1, (255,255,255), -1)
        if stt ==1:
	    cf.barrier = barrier
	    cf.time_barrier = time.time()
        if time.time() - cf.time_barrier>0.5:
	    cf.barrier = None
        img2[cut-1:cut+1,:] = 0
        img2[:, center-dis-1:center-dis+1] =0
        img2[:, center+dis-1:center+dis+1] = 0
	cf.depth = cv2.resize(img2, (480, int(img2.shape[0]*360/240)))
    print("Get_depth stoped")
