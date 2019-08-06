import cv2
import numpy as np

from p2c import p2c_main
from model_lane import Model


class AutoEncoder:
    def __init__(self, path, roi=0.5):
        """
        Initiate class
        :param path: path of road segmentation model
        :param roi: float number in range of [0, 1], represents the percentage of image height where ROI will be applied.
        ROI is a horizontal line that intersect with the road.
        Example: ROI = 0.5 means ROI line is at 50% of image height.
        """
        self.model = Model(path)
        self.roi = roi
        self.w = 160

    def get_points(self, img):
        """
        Get x coordinates of both sidelines of the road
        :param img: color RGB image
        :return:
            - x left: x coordinate of left sideline of the road
            - x right: x coordinate of right sideline of the road
            Note: if both x left and x right is 0 then there's no road side detected
        """
        segmented_img = self.model.predict(img)
        flood_filled_img = self.flood_fill(np.uint8(segmented_img))

        cv2.imshow('Segmented image', flood_filled_img * 255.)
        cv2.waitKey(1)

        flood_filled_img = flood_filled_img.astype(np.int32)
        x_left, x_right = p2c_main.get_center_points_by_roi(flood_filled_img, self.roi)

        return x_left, x_right

    def flood_fill(self, img):
        w, h = img.shape

        mask = np.zeros((h + 2, w + 2), np.uint8)
        bits = img[h - 2:h - 1, :]

        # ret, bits = cv2.threshold(bits, 0, 1, cv2.THRESH_BINARY)
        bounded = np.hstack(([0], bits.reshape(-1), [0]))
        diffs = np.diff(bounded)
        run_starts = np.where(diffs > 0)[0]
        run_ends = np.where(diffs < 0)[0]

        if len(run_starts) == 0:
            return img

        i = np.argmax(run_ends - run_starts, axis=0)
        seed = int((run_ends[i] + run_starts[i]) / 2)
        if img[h - 2][seed] == 0:
            print("Wrong flood fill!!!")
        # seed = 160
        cv2.floodFill(img, mask, (seed, h - 2), 1)
        # img_inv = cv2.bitwise_not(img)
        # fill_image = img | img_inv
        # cv2.imshow("my segment", mask*255.)
        return mask[1:161, 1:321]
