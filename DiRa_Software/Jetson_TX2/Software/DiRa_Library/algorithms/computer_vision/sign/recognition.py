import color_detection
import classification
from dira.utils.config import Config

SIGN_NONE = 0
SIGN_TURN_RIGHT = 1
SIGN_TURN_LEFT = 2

class SignRecognizer:
    def __init__(self):
        '''
        This class recognize traffic sign from a raw image.
        Main method: recognize_sign()
        '''
        self.color_detector = color_detection.ColorDetector()
        self.sign_classifier = classification.SignClassifier(Config().model_sign_path)

    def recognize_sign(self, img):
        """
        Detect and classify a traffic sign in the input image.

        :param img: numpy RGB image, not normalized (range 0-255)
        :return: 2 values, including:
            ___1. int sign type:
                0: SIGN_NONE
                1: SIGN_TURN_RIGHT
                2: SIGN_TURN_LEFT
            ___2. a tuple specifying sign position and size (x, y, w, h), in which (x,y) is the coordinate of the
            top left pixel of the traffic sign, w is width and h is height.

        Examples
        --------
        >>> detect_sign(rgb_frame)
        1, (60, 60, 100, 100)

        Warning
        ------
        Input image with a different colour space from RGB might give inaccurate result.
        """

        sign_imgs, points = self.color_detector.detect_by_color(img)

        for index in range(len(sign_imgs)):
            prediction = self.sign_classifier.predict_sign(sign_imgs[index])
            if prediction != 0:
                return prediction, points[index]

        return 0, (0, 0, 0, 0)
