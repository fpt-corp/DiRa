import cv2
import numpy as np

import tensorflow as tf

from .model_sign import model_sign


class SignClassifier:
    def __init__(self, path):
        self.sess = tf.Session(config=tf.ConfigProto(gpu_options=tf.GPUOptions(per_process_gpu_memory_fraction=0.02)))
        self.model = model_sign()
        self.model.load_weights(path)
        self.model._make_predict_function()

    def predict_sign(self, img):
        """
        Classify a traffic sign. Currently support 2 signs:
            1: TURN_RIGHT
            2: TURN_LEFT

        :param img: numpy RGB image of the traffic sign, not normalized (range 0-255)
        :return: int sign type:
            0: SIGN_NONE
            1: SIGN_TURN_RIGHT
            2: SIGN_TURN_LEFT

        Examples
        --------
        >>> sign_classifier.predict_sign(rgbimg)
        1

        Warning
        ------
        Input image with a different colour space from RGB might give inaccurate result.
        """

        img = self.preprocess(img)
        if np.isnan(img.sum()):
            return 0
        img = np.expand_dims(img, 0)

        output = self.model.predict(img)

        return output.argmax()

    def preprocess(self, img):
        """
        Preprocess an image for the classifier.

        :param img: numpy RGB image of the traffic sign, not normalized (range 0-255)
        :return: numpy gray-scale image with shape (height, width, 1)

        Examples
        -----
        >>> sign_classifier.preprocess(rgb_img)
        ndarray

        Warning
        ------
        Input image with a different colour space from RGB might give unexpected result.
        """
        img = cv2.resize(img, (32, 32))
        img = cv2.cvtColor(img, cv2.COLOR_RGB2YUV)

        # Convert to grayscale, e.g. single Y channel
        img = 0.299 * img[:, :, 0] + 0.587 * img[:, :, 1] + 0.114 * img[:, :, 2]

        # img = (img / 255.).astype(np.float32) #skimage
        # img = exposure.equalize_adapthist(img) #skimage

        img = np.array(img, dtype=np.uint8)  # cv2
        img = cv2.equalizeHist(img)  # cv2
        img = (img / 255.).astype(np.float32)  # cv2

        # Add a single grayscale channel
        img = np.expand_dims(img, -1)  # channel last

        return img  # (32, 32, 1)
