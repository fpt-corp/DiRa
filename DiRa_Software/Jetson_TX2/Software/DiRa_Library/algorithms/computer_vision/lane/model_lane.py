import numpy as np
import tensorflow as tf
from tensorflow.python.platform import gfile
from dira.utils.config import Config
# import tensorflow.contrib.tensorrt
# from keras.models import load_model

tf.keras.backend.set_learning_phase(0)


class Model:
    def __init__(self, path=None):
        if path is None:
            path = Config().model_lane_path

        self.graph = tf.Graph()
        self.sess = tf.Session(graph=self.graph,
                               config=tf.ConfigProto(gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.15)))

        with self.graph.as_default():
            trt_graph = self.read_pb_graph(path)
            tf.import_graph_def(trt_graph, name='')
            self.input = self.sess.graph.get_tensor_by_name('input_1:0')
            self.output = self.sess.graph.get_tensor_by_name('fcn21/truediv:0')
            self.feature = self.sess.graph.get_tensor_by_name('fcn18/my_trt_op_35:0')
            self.predict(np.zeros((160, 320, 3)))

    def read_pb_graph(self, path):
        with gfile.FastGFile(path, 'rb') as f:
            graph_def = tf.GraphDef()
            graph_def.ParseFromString(f.read())

        return graph_def

    def predict(self, image):
        image = np.expand_dims(image, 0)
        pred, f = self.sess.run([self.output, self.feature], feed_dict={self.input: image / 255.})
        pred = np.argmax(pred, axis=3)[0]

        return pred
