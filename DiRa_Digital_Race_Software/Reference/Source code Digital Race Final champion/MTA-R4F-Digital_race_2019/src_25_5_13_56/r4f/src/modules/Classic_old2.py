import cv2
import numpy as np
import matplotlib.pyplot as plt
import time
import config as cf
font = cv2.FONT_HERSHEY_SIMPLEX
def Pre(frame_):
    frame = frame_[100:320,:]
    black = np.zeros((220,480),np.uint8)
    gray = cv2.cvtColor(frame,cv2.COLOR_BGR2GRAY)
    mean = np.mean(gray)
    adj = mean*0.7
    if adj > 80:
        adj = 80
    if adj < 20:
        adj = 20
    mask_white = cv2.inRange(gray, mean+adj, 255)
    #cv2.imshow("mask_white", mask_white)
    adap = cv2.adaptiveThreshold(gray,255,cv2.ADAPTIVE_THRESH_MEAN_C,cv2.THRESH_BINARY,201,-50)
    #cv2.imshow("adap", adap)
    bitor = cv2.bitwise_or(mask_white, adap)
    kernel = np.ones((5,5),np.uint8)
    closing = cv2.morphologyEx(bitor, cv2.MORPH_CLOSE, kernel)
    __, contours,_ = cv2.findContours(closing,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    for cnt in contours:
        x,y,w,h = cv2.boundingRect(cnt)
        if w*2+h*2>280:
            cv2.drawContours(black, [cnt], -1, 255, -1)
    #cv2.imshow("black",black)
    return black
center_car = 100
def Classic(frame_, side_tracking, mode, dis, center_old, x):
    global center_car
    x1 = 320-x
    x2 = 320
    frame = frame_.copy()
    if side_tracking == 1:
        center_old = 480 - center_old
        frame = cv2.flip(frame,1)
    img = frame.copy() #img show
    frame = cv2.resize(frame,(480,320)) 
    frame = Pre(frame)
    img2 = frame.copy()
    frame = frame[frame.shape[0]-x:frame.shape[0],:]
    if mode == 0:
        __, contours,_ = cv2.findContours(frame,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
        obj = np.zeros((2,4)) #obj[row,col]
        stt = 0
        center = center_old
        scnt = 0
        for cnt in contours:
            xct,yct,wct,hct = cv2.boundingRect(cnt)
            #cv2.putText(img,str(wct)+" "+str(hct), (xct, yct+250), font, 1,(255,255,255),3,cv2.LINE_AA)
            rect = cv2.minAreaRect(cnt)
            box = cv2.boxPoints(rect)
            box = np.int0(box)
            box2 = box.copy()
            box2[:,1] += 320 - x
            block_w = np.sqrt((box2[0][0]-box2[1][0])**2+(box2[0][1]-box2[1][1])**2)
            block_h = np.sqrt((box2[0][0]-box2[3][0])**2+(box2[0][1]-box2[3][1])**2)
            if block_w > block_h:
                stemp = block_h
                block_h = block_w
                block_w = stemp
            scnt = cv2.contourArea(cnt)
            if (scnt<5000) and (block_w>10) and (block_w<80) and (block_h >50) and (stt<3):
                cv2.drawContours(img,[box2],0,(255,130,171),-1)
                if box[1][1]<box[3][1]:
                    center_high = int((box[1][0]+box[2][0])/2)
                    center_low = int((box[0][0]+box[3][0])/2)
                else:
                    center_high = int((box[3][0]+box[2][0])/2)
                    center_low = int((box[0][0]+box[1][0])/2)
                obj[:,stt] = center_high, center_low
                cv2.putText(img,str(int(block_w))+"x"+str(int(block_h)),(center_high-30, 220), font, 1,(255,255,255),3,cv2.LINE_AA)
                #cv2.circle(img,(center_high,x1), 5, (96, 164, 244), -1)
                #cv2.circle(img,(center_low,x2), 5, (96, 164, 244), -1)
                stt += 1
        
        if stt == 1:
            if obj[1][0] <= 380:
                center = int(obj[0,0]) + dis
        if stt == 2:
            lane1 = int(obj[0,0])
            lane2 = int(obj[0,1])
            if (obj[1][0]-380)*(obj[1][1]-380)<0: #2 line khac phia
                center = min(lane1,lane2) + dis
            else:
                center = max(lane1,lane2) + dis
    else:
        frame = frame[0:20,:]
        #cv2.imshow("frame",frame)
        __, contours,_ = cv2.findContours(frame,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
        obj = np.zeros((4,4)) #obj[row,col]
        stt = 0
        center = center_old
        x_min = 1000
        x_max = 0
        xlim = 420
        save = np.zeros((4,1))
        if mode == 1:
            for cnt in contours:
                _x,_y,_w,_h = cv2.boundingRect(cnt)
                if time.time()-cf.time_stop >0.5 and _w > 130:
                    cf.time_stop = time.time()
		    cf.stopping = True
                #if w>10:
                if _x<x_min and _x<xlim:
                    x_min = _x
                    save = _x,_y,_w,_h
            if len(contours) != 0:
                center = x_min + dis
                cv2.putText(img,str(int(_w))+"x"+str(int(_h)),(center-30, 220), font, 1,(255,255,255),3,cv2.LINE_AA)
                cv2.rectangle(img,(save[0],save[1]+x1-10),(save[0]+save[2],save[1]+save[3]+x1-10),(124,243,196),-1)
            
        else:
            for cnt in contours:
                _x,_y,_w,_h = cv2.boundingRect(cnt)
                #if w>10:
                if _x+_w>x_max and _x+_w<xlim:
                    x_max = _x+_w
                    save = _x,_y,_w,_h
            if len(contours) != 0:
                center = x_max + dis
                cv2.rectangle(img,(save[0],save[1]+x1-10),(save[0]+save[2],save[1]+save[3]+x1-10),(124,243,196),-1)
        cv2.line(img,(xlim,250),(xlim,360),(255,0,0),5)
    #cv2.rectangle(img,(0,x1),(480,x2),(255,0,0),1)
    #cv2.putText(img,str(lane_old),(240, 100), font, 1,(255,255,255),3,cv2.LINE_AA)
    cv2.circle(img,(center,250), 10, (0,255,0), -1)
    if side_tracking == 1:
        center = 480 - center
        img = cv2.flip(img,1)
    return img, img2, center
