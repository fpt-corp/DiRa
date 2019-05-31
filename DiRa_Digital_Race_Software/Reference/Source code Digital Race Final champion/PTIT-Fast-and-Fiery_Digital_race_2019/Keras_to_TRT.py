import tensorflow as tf
tf.keras.backend.set_learning_phase(0)
MODEL_PATH = "./tensorRT/model"
import tensorflow.contrib.tensorrt as trt
from tensorflow.python.platform import gfile
import cv2
import numpy as np
import time
from tensorflow.keras.models import model_from_json
from tensorflow.keras.utils import CustomObjectScope
from tensorflow.keras.initializers import glorot_uniform

PRECISION = "FP32"

def load_keras_model(file_name):
    f = open('model-mobilenet-iter2-pretrain-data-bdd.json', 'r')
    with CustomObjectScope({'GlorotUniform': glorot_uniform()}):
        model = model_from_json(f.read())
    model.load_weights(file_name + '.h5')
    print "Model loaded"
    return model

def keras_to_TF():
    saver = tf.train.Saver()
    sess = tf.keras.backend.get_session()
    save_path = saver.save(sess, "./tensorRT/model")
    print("Keras model is successfully converted to TF graph in " + save_path)
    sess.close()


def TF_to_TRT():
    with tf.Session(config=tf.ConfigProto(gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.5))) as sess:
        saver = tf.train.import_meta_graph("./tensorRT/model.meta")
        saver.restore(sess, "./tensorRT/model")
        your_outputs = ["fcn17/truediv"]

        frozen_graph = tf.graph_util.convert_variables_to_constants(sess, tf.get_default_graph().as_graph_def(),
                                                                    output_node_names=your_outputs)
        with gfile.FastGFile("./tensorRT/frozen_model.pb", 'wb') as f:
            f.write(frozen_graph.SerializeToString())
        print("Frozen model is successfully stored!")
    trt_graph = trt.create_inference_graph(
        input_graph_def=frozen_graph,
        outputs=your_outputs,
        max_batch_size=1,
        max_workspace_size_bytes=2500000000,
        precision_mode=PRECISION)

    with gfile.FastGFile("./tensorRT/TensorRT_5_" + PRECISION + ".pb", 'wb') as f:
        f.write(trt_graph.SerializeToString())
    print("TensorRT model is successfully stored!")
    all_nodes = len([1 for n in frozen_graph.node])
    print("numb. of all_nodes in frozen graph:", all_nodes)
    trt_engine_nodes = len([1 for n in trt_graph.node if str(n.op) == 'TRTEngineOp'])
    print("numb. of trt_engine_nodes in TensorRT graph:", trt_engine_nodes)
    all_nodes = len([1 for n in trt_graph.node])
    print("numb. of all_nodes in TensorRT graph:", all_nodes)
    return trt_graph

def read_pb_graph(model):
    with gfile.FastGFile(model,'rb') as f:
        graph_def = tf.GraphDef()
        graph_def.ParseFromString(f.read())
    return graph_def

def test(n_time_inference=50):
    input_img = np.zeros((1, 160, 320, 3))
    graph = tf.Graph()
    with graph.as_default():
        with tf.Session(config=tf.ConfigProto(gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.5))) as sess:
            trt_graph = read_pb_graph('./tensorRT/TensorRT_5_'+ PRECISION + '.pb')
            tf.import_graph_def(trt_graph, name='')
            input = sess.graph.get_tensor_by_name('input_1:0')
            output = sess.graph.get_tensor_by_name('fcn17/truediv:0')
            total_time = 0;
            out_pred = sess.run(output, feed_dict={input: input_img})
            for i in range(n_time_inference):
                t1 = time.time()
                out_pred = sess.run(output, feed_dict={input: input_img})
                t2 = time.time()
                delta_time = t2 - t1
                total_time += delta_time
            avg_time_tensorRT = total_time / n_time_inference
            print "average inference time: ", avg_time_tensorRT

    graph = tf.Graph()
    with graph.as_default():
        with tf.Session() as sess:
            frozen_graph = read_pb_graph('./tensorRT/frozen_model.pb')
            tf.import_graph_def(frozen_graph, name='')
            input = sess.graph.get_tensor_by_name('input_1:0')
            output = sess.graph.get_tensor_by_name('fcn17/truediv:0')
            total_time = 0;
            out_pred = sess.run(output, feed_dict={input: input_img})
            for i in range(n_time_inference):
                t1 = time.time()
                out_pred = sess.run(output, feed_dict={input: input_img})
                t2 = time.time()
                delta_time = t2 - t1
                total_time += delta_time
            avg_time_original_model = total_time / n_time_inference
            print "average inference time: ", avg_time_original_model
            print "TensorRT improvement compared to the original model:", avg_time_original_model / avg_time_tensorRT


def pipe_line(keras_model_path):
    load_keras_model(keras_model_path)
    keras_to_TF()
    TF_to_TRT()
    test()

pipe_line("model-mobilenet-5")


