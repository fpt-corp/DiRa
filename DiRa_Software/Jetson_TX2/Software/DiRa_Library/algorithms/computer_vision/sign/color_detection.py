import cv2
import numpy as np


class ColorDetector:
    def __init__(self, hsv_color_ranges=((100, 120),), min_saturation=127,
                 min_value=100, min_sign_size=50,max_sign_size=200, differential_rate=0.2):
        '''
        This class detect color regions from raw image.
        Main method: detect_by_color()

        Initialize a color detector.
        :param hsv_color_ranges: sequence of color ranges to be detected. Each range is comprised of lower bound and upper bound.
        :param min_saturation: the minimum saturation to be detected.
        :param min_value: the minimum value (or brightness) to be detected.
        :param min_sign_size: the minimum size (in pixel) of traffic sign to be detected.
        :param max_sign_size: the maximum size (in pixel) of traffic sign to be detected.
        :param differential_rate: the maximum differential rate between height and width of the color to be detected. If height=width then rate=0.
            This rate is computed using the formula: |width-height| / [(width+height)/2]
        '''

        self.hsv_color_ranges = hsv_color_ranges
        self.min_saturation = min_saturation
        self.min_value = min_value
        self.min_sign_size = min_sign_size
        self.max_sign_size= max_sign_size
        self.differential_rate = differential_rate

    def detect_by_color(self, img_np):
        """
        Detect objects in the input image via color-based method.

        :param img_np: numpy RGB image, not normalized (range 0-255)

        :return: 2 lists:
            ___1. List of numpy rgb images of color-matched regions.
            ___2. List of the respectively region's position and size. Each element of the list is a tuple (x,y,w,h), in which (x,y) is the coordinate of the top left pixel of the region, w is width and h is height.

        Examples
        ------
        >>> find_by_color(rgb_frame)
        (np.array, np.array), ((60,60,100,100), (80,80,30,30), )

        Warning
        ------
        Input image with a different color space from RGB might give bad result.
        """
        img = np.copy(img_np)
        hsv = cv2.cvtColor(img, cv2.COLOR_RGB2HSV)
        combined = cv2.inRange(hsv, (0, 0, 0), (0, 0, 0))
        for range in self.hsv_color_ranges:
            a_color = cv2.inRange(hsv, (range[0], self.min_saturation, self.min_value), (range[1], 255, 255))
            combined = cv2.bitwise_or(combined, a_color)

        combined = cv2.blur(combined, (3, 3))

        contours = cv2.findContours(combined, cv2.RETR_EXTERNAL, cv2.CHAIN_APPROX_SIMPLE)[-2]
        list_sign_crop = []
        list_point = []
        for cnt in contours:
            (x, y, w, h) = cv2.boundingRect(cnt)
            if (w * h < self.min_sign_size ** 2) or (w*h>self.max_sign_size**2) or (abs(float(w - h) / ((w + h) / 2.)) > self.differential_rate):
                continue
            list_sign_crop.append(img[y:y + h, x:x + w])

            list_point.append((x, y, w, h))

        return list_sign_crop, list_point
