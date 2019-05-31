print ("Import Line module")
import cv2
import numpy as np

def Line(frame):
    center = 0
    #dst = np.zeros_like(frame)
    gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    mean = np.mean(gray)
    adj = mean*0.7
    if adj > 80:
        adj = 80
    if adj < 20:
        adj = 20
    mask_white = cv2.inRange(gray, mean+adj, 255)
    #mask_white = mask_white[100:120,:] #(20,480)
    __, contours,_ = cv2.findContours(mask_white,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    for cnt in contours:
        if (cv2.contourArea(cnt) > 150):
            x,y,w,h = cv2.boundingRect(cnt)
            center = int(x+w/2)
    return center
