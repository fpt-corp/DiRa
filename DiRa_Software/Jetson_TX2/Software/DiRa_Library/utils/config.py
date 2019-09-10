import os


class Config:
    def __init__(self):
        self.path = os.getcwd()

        self.model_lane_path = self.path + '/models/lane.pb'
        self.model_sign_path = self.path + '/models/sign.h5'
        self.p2c_path = self.path + '/p2c'
