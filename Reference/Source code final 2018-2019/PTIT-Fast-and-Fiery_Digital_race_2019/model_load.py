#!/usr/bin/env python
from tensorflow.keras.layers import *
from tensorflow.keras.models import Model
from tensorflow.keras import applications
import cv2
import numpy as np
import rospy
import rospkg
from std_msgs.msg import String, Float32, Bool
from sensor_msgs.msg import CompressedImage
from steer import SegmentToSteer
rospack = rospkg.RosPack()
path = rospack.get_path('team107') + '/scripts/'
import tensorflow as tf
from keras.backend.tensorflow_backend import set_session
config = tf.ConfigProto()
config.gpu_options.allow_growth = True
sess = tf.Session(config=config)
set_session(sess)
mbl = applications.mobilenet.MobileNet(weights=None, include_top=False, input_shape=(160,320,3))
x = mbl.output

model_tmp =  Model(inputs = mbl.input, outputs = x)
layer5, layer8, layer13 = model_tmp.get_layer('conv_pw_5_relu').output, model_tmp.get_layer('conv_pw_8_relu').output, model_tmp.get_layer('conv_pw_13_relu').output

fcn14 = Conv2D(filters=2 , kernel_size=1, name='fcn14')(layer13)
fcn15 = Conv2DTranspose(filters=layer8.get_shape().as_list()[-1] , kernel_size=4, strides=2, padding='same', name='fcn15')(fcn14)
fcn15_skip_connected = Add(name="fcn15_plus_vgg_layer8")([fcn15, layer8])
fcn16 = Conv2DTranspose(filters=layer5.get_shape().as_list()[-1], kernel_size=4, strides=2, padding='same', name="fcn16_conv2d")(fcn15_skip_connected)
# Add skip connection
fcn16_skip_connected = Add(name="fcn16_plus_vgg_layer5")([fcn16, layer5])
# Upsample again
fcn17 = Conv2DTranspose(filters=2, kernel_size=16, strides=(8, 8), padding='same', name="fcn17", activation="softmax")(fcn16_skip_connected)

model = Model(inputs = mbl.input, outputs = fcn17)

model.load_weights(path + 'model-mobilenet-data-bdd-pretrained-carla.h5')
cap = cv2.VideoCapture(path + 'video_2.avi')
# with CustomObjectScope({'GlorotUniform': glorot_uniform()}):
#     model = load_model("mobilenet.h5py")
#     model_1 = load_model("mobilenet_bdd.h5py")
s2s = SegmentToSteer()
is_running = False


def engine_callback(data):
    global is_running
    is_running = data


def publish_data(speed_val, steerAngle_val):
    global pub_speed
    global pub_steer
    pub_speed.publish(speed_val)
    pub_steer.publish(steerAngle_val)


#sub_ss = rospy.Subscriber('/ss_status', Bool, engine_callback, queue_size=1)
# sub_img = rospy.Subscriber('/camera/rgb/image_raw/compressed', CompressedImage, run_callback, queue_size=1)
pub_speed = rospy.Publisher('/set_speed_car_api', Float32, queue_size=1)
pub_steer = rospy.Publisher('/set_steer_car_api', Float32, queue_size=1)

rospy.init_node('test')

while(True):
    # Capture frame-by-frame
    ret, frame = cap.read()

    # Display the resulting frame
    frame = cv2.resize(frame[:500, :], (320, 160))
    gray = cv2.cvtColor(frame, cv2.COLOR_BGR2GRAY)
    gray = np.expand_dims(frame, axis=0)
    # gray = np.expand_dims(frame, axis=3)
    seg = model.predict(gray/255.)[0]
    seg = np.argmax(seg, axis=2)
    # frame[seg==1] = [0, 0, 255]
    seg *= 255
    speed, steer, overlap = s2s.get_steer(frame, seg, 0, 1)
    cv2.imshow('frame', overlap)
    print speed, steer
    publish_data(speed, steer)
    if cv2.waitKey(1) & 0xFF == ord('q'):
        break

