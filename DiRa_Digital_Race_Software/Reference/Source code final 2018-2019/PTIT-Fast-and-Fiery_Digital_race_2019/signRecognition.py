from imutils.perspective import four_point_transform
from imutils import contours
import imutils
import cv2
import numpy as np
from keras.models import load_model
import rospkg
rospack = rospkg.RosPack()
path = rospack.get_path('team107') + '/scripts/'
model = load_model(path + 'updated.h5')
model._make_predict_function()

def over_lap_area(r1, r2):
    x1_left, y1_top, w1, h1 = r1
    x1_right = x1_left + w1
    y1_bottom = y1_top + h1
    x2_left, y2_top, w2, h2 = r2
    x2_right = x2_left + w2
    y2_bottom = y2_top + h2

    s = max(0, (min(x1_right+1, x2_right+1) - max(x1_left-1, x2_left-1)))*max(0,(min(y1_bottom-1, y2_bottom-1) - max(y1_top+1, y2_top+1)))
    
    return s
    
def merge_rects(r1, r2):
    x1_left, y1_top, w1, h1 = r1
    x1_right = x1_left + w1
    y1_bottom = y1_top + h1
    x2_left, y2_top, w2, h2 = r2
    x2_right = x2_left + w2
    y2_bottom = y2_top + h2

    return min(x1_left, x2_left), min(y1_top, y2_top), max(x1_right, x2_right) - min(x1_left, x2_left), max(y1_bottom, y2_bottom) - min(y1_top, y2_top)

def optimize(rects):
    while True:
        changed = 0
        for i in range(len(rects)):
            for j in range(i+1, len(rects)):
                r1 = rects[i]
                r2 = rects[j]
                if over_lap_area(r1, r2) > 0:
                    rects.append(merge_rects(r1, r2))
                    rects.remove(r1)
                    rects.remove(r2)
                    changed = 1
                    break
            if changed == 1:
                break
        if changed == 0:
            break
    return rects

def detect1(frame):
    global model
    # frame = cv2.imread('/home/pham.hoang.anh/prj/traffic_sign/data/1.jpg')
    # gray = cv2.imread('/home/pham.hoang.anh/prj/traffic_sign/data/1.jpg', 0)
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    hsv = cv2.cvtColor(frame, cv2.COLOR_BGR2HSV)

    light_blue = (98, 109, 20)
    dark_blue = (112, 255, 255)

    mask = cv2.inRange(hsv, light_blue, dark_blue)
    _, cnts, hierarchy = cv2.findContours(mask, cv2.RETR_TREE, cv2.CHAIN_APPROX_SIMPLE)

    arr = []

    for cnt in cnts:
        (x, y, w, h) = cv2.boundingRect(cnt)
        ratio = w/float(h)
        #check CIRCLE, MIN WIDTH, MIN HEIGHT
        if ratio > 0.8 and ratio < 1.2 and w > 5 and w < 120:
            arr.append((x, y, x+w, y+h))

    arr = optimize(arr)

    predict_result = []
    s = []


    if len(arr) != 0:
        for a in arr:
            detect = gray[a[1]:a[3], a[0]:a[2]]
            detect = cv2.resize(detect, (24, 24))
            detect = np.expand_dims(detect, axis = 3)
            detect = np.expand_dims(detect, axis = 0)
            pred = model.predict(detect)
            predict_result.append(pred)
            s.append((a[2] - a[0]) * (a[3] -a[1]))
    
        max_predict = np.max(predict_result)
        arg_max = np.argmax(predict_result)
        s_max = s[arg_max]
        min_predict = np.min(predict_result)
        arg_min = np.argmin(predict_result)
        s_min = s[arg_min]
        if max_predict < 0.87 and min_predict > 0.13:
			return 0, 1
        elif max_predict > 1 - min_predict:
            return -1, s_max
        elif max_predict < 1 - min_predict:
            return 1, s_min
    return 0, 0
    # cv2.imshow('image', result)
    # cv2.waitKey(1)


