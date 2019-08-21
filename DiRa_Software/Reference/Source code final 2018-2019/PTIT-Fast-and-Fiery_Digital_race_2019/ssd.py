import tensorflow as tf
from keras.models import load_model
import cv2
import numpy as np


class Detector(object):
    def __init__(self, path):
        gpu_options = tf.GPUOptions(per_process_gpu_memory_fraction=0.12)
        self.session = tf.Session(config=tf.ConfigProto(gpu_options=gpu_options))
        saver = tf.train.import_meta_graph(path + 'output/' + "model.ckpt.meta")
        saver.restore(self.session, path + 'output/' + "model.ckpt")
        ops = self.session.graph.get_operations()
        all_tensor_names = {output.name for op in ops for output in op.outputs}
        classification_model = path + 'updated_1.h5'
        self.tensor_dict = {}
        self.clf = load_model(classification_model)
        self.clf._make_predict_function()
        for key in ['num_detections', 'detection_boxes', 'detection_scores', 'detection_classes']:
            tensor_name = key + ':0'
            if tensor_name in all_tensor_names:
                self.tensor_dict[key] = self.session.graph.get_tensor_by_name(tensor_name)

        self.image_tensor = self.session.graph.get_tensor_by_name('image_tensor:0')

    def predict(self, image):
        w = 640
        h = 480
        image = cv2.cvtColor(image, cv2.COLOR_BGR2RGB)
        output_dict = self.session.run(self.tensor_dict, feed_dict={self.image_tensor: np.expand_dims(image, 0)})
        output_dict['num_detections'] = int(output_dict['num_detections'][0])
        output_dict['detection_classes'] = output_dict['detection_classes'][0].astype(np.uint8)
        output_dict['detection_boxes'] = output_dict['detection_boxes'][0]
        output_dict['detection_scores'] = output_dict['detection_scores'][0]
        bnbbox = output_dict['detection_boxes']
        # classes = predictions['detection_classes']
        scores = output_dict['detection_scores']
        image = cv2.cvtColor(image, cv2.COLOR_RGB2BGR)

        if scores[0] > 0.01:
            x1, y1, x2, y2 = int(bnbbox[0][1] * w), int(bnbbox[0][0] * h), int(bnbbox[0][3] * w), int(bnbbox[0][2] * h)

            area = image.copy()[y1:y2, x1:x2]
            area = cv2.resize(area, (48, 48))
            area = np.expand_dims(area, axis=0)

            score = self.clf.predict(area)[0][0]

            if score < 0.2:
                return -1, 0
            elif score > 0.8:
                return 1, 0
        return 0, 0
