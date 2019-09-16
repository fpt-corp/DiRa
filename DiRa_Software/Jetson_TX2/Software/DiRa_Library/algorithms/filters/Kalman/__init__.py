'''
=============
Kalman Module
=============

This module provides inference methods for state-space estimation in continuous
spaces.
'''

from DiRa_Library.algorithms.filters.Kalman.standard import KalmanFilter
from DiRa_Library.algorithms.filters.Kalman.unscented import AdditiveUnscentedKalmanFilter, UnscentedKalmanFilter

__all__ = [
    "KalmanFilter",
    "AdditiveUnscentedKalmanFilter",
    "UnscentedKalmanFilter",
    "datasets",
    "sqrt"
]
