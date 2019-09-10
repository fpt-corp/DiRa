import numpy as np
import numpy.ctypeslib as npct
from ctypes import *
import ctypes as ct
# import get_point as gp
# import random
# import time

from dira.utils.config import Config
path = Config().p2c_path
print(path)
libc = npct.load_library('p2c', path)

rows = 160
cols = 320
array_2d_int = npct.ndpointer(dtype=np.int32, ndim=2, flags='CONTIGUOUS')
libc.get_center_points_by_roi.argtypes = [array_2d_int, c_float, POINTER(c_int), POINTER(c_int)]
libc.get_point.argtypes = [array_2d_int, c_float, POINTER(c_int), POINTER(c_int), c_int, c_int, c_int, c_bool, c_int]
libc.check_future_road.argtypes = [array_2d_int, c_float, POINTER(c_bool), POINTER(c_int), c_int]


def __get_point(img, roi, flag, left_restriction, right_restriction, has_road, road_property):
    """
    This method is decapitated.
    Get road center point by using a horizontal line (ROI) to find intersection points with segmented image
    :param img: 2D array segmented image contains only 0s and 1s as values.
    :param roi: float number in range of [0, 1], represents the percentage of image height where ROI will be applied.
        Example: ROI = 0.5 meaning the line is at 50% of image height.
    :param flag: status of traffic sign.
        - -1 if turn left
        - 1 if turn right
        - 0 otherwise
    :param left_restriction: integer number.
        - > 0 if there's obstacle on the left
        - 0 if there is none
    :param right_restriction: integer number.
        - > 0 if there's obstacle on the right
        - 0 if there is none
    :param has_road: boolean
        - True if has road beyond ROI.
        - False otherwise
    :param road_property: integer number, indicate property of the road beyond ROI.
        - -1 if the road has the trend of going left
        - 0 if the road has the trend of going straight
        - 1 if the road has the trend of going right
    :return: coordinate of center point:
        - x: x coordinate of center point
        - y: y coordinate of center point
    """
    # libc.get_point(a, 0.6, p_x, p_y, 0, 0, 319)
    # x = c_int(0)
    # y = c_int(0)
    p_x = pointer(c_int(0))
    p_y = pointer(c_int(0))
    libc.get_point(img, roi, p_x, p_y, flag, left_restriction, right_restriction, has_road, road_property)
    return p_x.contents.value, p_y.contents.value


def __check_future_road(img, roi, margin):
    """
    This method is decapitated.
    Get property of the road by using a horizontal line (ROI) to find intersection points with segmented image
    :param img: 2D array segmented image contains only 0s and 1s as values.
    :param roi: float number in range of [0, 1], represents the percentage of image height where ROI will be applied.
        Example: ROI = 0.5 means ROI line is at 50% of image height.
    :param margin: integer number, is the maximum distance from one side of the road to it corresponding image border in
    which that side is considered to have the trend of going to that direction.
    :return:
        - road property: integer number.
            - -1 if the road has the trend of going left
            - 0 if the road has the trend of going straight
            - 1 if the road has the trend of going right
        - has road: boolean
            - True if has road.
            - False otherwise
    """
    p_has_road = pointer(c_bool())
    p_road_property = pointer(c_int(0))
    libc.check_future_road(img, roi, p_has_road, p_road_property, margin)
    return p_road_property.contents.value, p_has_road.contents.value


def get_center_points_by_roi(img, roi):
    """
    Get x coordinates of both sidelines of the road by using a horizontal line (ROI) to find intersection points with
    segmented image
    :param img: 2D array segmented image contains only 0s and 1s as values.
    :param roi: float number in range of [0, 1], represents the percentage of image height where ROI will be applied.
        Example: ROI = 0.5 means ROI line is at 50% of image height.
    :return:
        x left: x coordinate of left side of the road
        x right: x coordinate of right side of the road
    """
    p_x_left = pointer(c_int())
    p_x_right = pointer(c_int())
    libc.get_center_points_by_roi(img, roi, p_x_left, p_x_right)
    return p_x_left.contents.value, p_x_right.contents.value
