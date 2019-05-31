import tensorflow as tf
from Keras_to_TRT import read_pb_graph, inference
import cv2
sess = tf.Session(config=tf.ConfigProto(gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.5)))
trt = read_pb_graph("./TensorRT_FP32.pb")
cap = cv2.VideoCapture('output.avi')
tf.import_graph_def(trt, name='')
ret, frame = cap.read()
input = sess.graph.get_tensor_by_name('input_1:0')
output = sess.graph.get_tensor_by_name('fcn17/truediv:0')
frame = cv2.resize(frame[:500, :], (320, 160))
frame = inference(sess, frame, input, output)
n_frame = 1
total_time = 0
import time
while (True):
    ret, frame = cap.read()
    t1 = time.time()
    frame = cv2.resize(frame[:500, :], (320, 160))

    out_pred = inference(sess, frame, input, output)
    frame[out_pred == 1] = [0, 0, 255]

    t2 = time.time()

    cv2.imshow("frame", frame)
    delta = t2 - t1
    total_time += delta
    if n_frame % 50 == 0:
        print n_frame/total_time
    n_frame += 1
    cv2.waitKey(1)
