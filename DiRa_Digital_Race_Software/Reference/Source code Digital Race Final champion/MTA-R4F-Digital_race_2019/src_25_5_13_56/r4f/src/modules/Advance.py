print ("Import Advance module")
import cv2
import numpy as np
import tensorflow as tf
from keras.models import load_model
import rospkg 
rospack = rospkg.RosPack()
path = rospack.get_path('r4f')
class loadModel:
    @staticmethod
    def loadmodel(path):
        return load_model(path)

    def __init__(self, path):
       self.model = self.loadmodel(path)
       self.graph = tf.get_default_graph()

    def predict(self, X):
        with self.graph.as_default():
            return self.model.predict(X)

crop =np.zeros((64,64),np.uint8)
config = tf.ConfigProto()
config.gpu_options.allow_growth = True
session = tf.Session(config=config)
lane_model = loadModel(path+'/models/model.h5')
#sign_model = loadModel(path+'/models/sign.h5')
speed_model = loadModel(path+'/models/speed.h5')
def Advance_lane(img):
    global lane_model
    img = cv2.resize(img, (240, 160))
    predict = lane_model.predict(np.array([img/255.0]))[0]
    center = int(predict[0]*480)
    return center

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

def Advance_sign(img_, colors): 
    global sign_model, crop    
    t = 5
    side = None
    img = img_[0:130,...].copy()
    img2 = img_[0:130,...].copy()
    mask = np.zeros_like(img, np.uint8)[:, :, 0]
    for color in colors:
        lower_blue, upper_blue = convertHSV(color)
        hsv = cv2.cvtColor(img2, cv2.COLOR_BGR2HSV)
        mask_blue = cv2.inRange(hsv, np.array(lower_blue), np.array(upper_blue))
        mask[mask_blue==255]=255
    mask = np.array(mask)
    im2, contours, hierarchy = cv2.findContours(mask,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > 50:
            x,y,w,h = cv2.boundingRect(cnt)
            x = max(0, x-t)
            y = max(0, y -t)
            cv2.rectangle(img2,(x,y),(x+w+2*t,y+h+2*t),(255,0,0),-1)
    lower_blue = np.array([110,220,220])
    upper_blue = np.array([130,255,255])
    hsv = cv2.cvtColor(img2, cv2.COLOR_BGR2HSV)
    mask_blue = cv2.inRange(hsv, lower_blue, upper_blue)
    im2, contours, hierarchy = cv2.findContours(mask_blue,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
    
    for cnt in contours:
        area = cv2.contourArea(cnt)
        if area > 400:
            x,y,w,h = cv2.boundingRect(cnt)
            if x>5 and y>5 and x+w<475 and y+h<125 and 1.0*h/w>0.85 and 1.0*h/w<1.3 :
                position = [x, y, w, h]
                crop = img[y:y+h, x:x+w,  :]
                crop = cv2.resize(crop, (64, 64))
                _side, confident = sign_model.predict(np.array([crop])/255.0)[0]
                if _side<0.5:
                    _side = 0
                else:
                    _side = 1
                if confident>0.5:
                    side = _side
                cv2.rectangle(img,(x,y),(x+w,y+h),(0,255,0),2)
                font = cv2.FONT_HERSHEY_SIMPLEX
                cv2.putText(img,str(confident),(x-10,y-10), font, 0.5, (255,255,255), 1, cv2.LINE_AA)
    return crop, side, img
sign_cascade = cv2.CascadeClassifier(path+'/models/cascade2.xml')
def FindSign(img):
    signs = sign_cascade.detectMultiScale(img,
    scaleFactor=1.03,
    minNeighbors=2,
    minSize=(19, 19),
    maxSize=(60, 60),
    flags = 0)
    return signs
	
def Cascade_sign(img_, colors):  
    global sign_model, crop
    t = 3	
    side = None
    img = img_[0:100,...].copy()
    img2 = img_[0:100,...].copy()
    signs = FindSign(img2)
    for (x,y,w,h) in signs:
        if x>5 and y>5 and x+w<475 and y+h<95 and 1.0*h/w>0.85 and 1.0*h/w<1.3:
            position = [x, y, w, h]
            crop = img[y-t:y+h+t, x-t:x+w+t,  :]
            crop = cv2.resize(crop, (64, 64))
            _side, confident = sign_model.predict(np.array([crop])/255.0)[0]
            if _side<0.5:
                _side = 0
            else:
                _side = 1
            if confident>0.5:
                side = _side
            cv2.rectangle(img,(x-t,y-t),(x+w+t,y+h+t),(0,255,0),2)
            font = cv2.FONT_HERSHEY_SIMPLEX
            cv2.putText(img,str(confident),(x-10,y-10), font, 0.5, (255,255,255), 1, cv2.LINE_AA)
    return crop, side, img

def Speed_prediction(img):
	img = cv2.resize(img, (240, 160))
	speed = speed_model.predict(np.array([img])/255.0)[0]
	return speed
