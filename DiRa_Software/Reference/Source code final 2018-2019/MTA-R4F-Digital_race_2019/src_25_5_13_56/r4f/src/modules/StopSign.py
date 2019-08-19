
import cv2
import numpy as np
def convertHSV(color, h=10, s=30, v=40):
    R = color[0]
    G = color[1]
    B = color[2]
    img = np.zeros((1, 1, 3), np.uint8)
    img[:, :] = [B, G, R]
    hsv = cv2.cvtColor(img, cv2.COLOR_BGR2HSV)[0,0]
    low = [hsv[0]-h, hsv[1]-s, hsv[2]-v]
    high = [hsv[0]+h, hsv[1]+s, hsv[2]+v]
    return low, high

colors = [(2, 59, 153)]

def Get_sign_position(frame):
    frame = cv2.resize(frame, (320, 240))
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    mask = np.zeros_like(frame, np.uint8)[:, :, 0]
    for color in colors:
        lower_blue, upper_blue = convertHSV(color)
        mask_blue = cv2.inRange(hsv, np.array(lower_blue), np.array(upper_blue))
        mask[mask_blue==255]=255
    mask = np.array(mask)
    im2, contours, hierarchy = cv2.findContours(mask,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    stt = 0
    x = y = w = h = 0
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > 50:
            stt+=1
            x,y,w,h = cv2.boundingRect(cnt)
            cv2.rectangle(frame, (x, y), (x+w, y+h), (255, 0, 0), -1)
    lower_blue = np.array([110,220,220])
    upper_blue = np.array([130,255,255])
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)
    mask_blue = cv2.inRange(hsv, lower_blue, upper_blue)
    im2, contours, hierarchy = cv2.findContours(mask_blue,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    stt = 0
    position_array = []
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > 400:
	    x,y,w,h = cv2.boundingRect(cnt)
            position_array.append([x, y, w, h])
    return np.array(position_array), frame


def Get_sign_distance(img, depth):
    position_array,frame = Get_sign_position(img)
    distance_array = []
    for object in position_array:
	x, y, w, h = object
	if x>120 and x<240 and y<110 and 1.0*w/h<1.3 and 1.0*w/h>0.8  :
		distance= np.mean(depth[y+h//2-5:y+h//2+5, x+w//2-5:x+w//2+5])
		if distance >250:
			distance = 0
		distance_array.append(distance)
		cv2.rectangle(frame, (x, y), (x+w, y+h), (0, 255, 0), 1)
    if len(distance_array)==0:
	return 0, frame
    return np.max(distance_array),frame
