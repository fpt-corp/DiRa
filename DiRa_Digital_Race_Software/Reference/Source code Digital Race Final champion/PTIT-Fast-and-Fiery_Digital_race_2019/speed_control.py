import numpy as np
class SpeedController():
    def __init__(self, speed_max, speed_min, speed_accel, break_accel):
        self.speed_max = speed_max
        self.speed_min = speed_min
        self.speed_accel = speed_accel
        self.break_accel = break_accel
        self.last_speed = 0
        self.total_time = 0.0
        self.counter = 0

    def control_speed(self, steer, interval):
	self.total_time += interval
        self.counter +=1
        avg_time = self.total_time / self.counter
        desired_speed = (self.speed_min -self.speed_max) * steer**2/ 3600 + self.speed_max
        changes = desired_speed - self.last_speed
        accel = self.speed_accel
        if np.sign(changes) < 0:
            accel = self.break_accel
        self.last_speed += np.sign(changes) * min(abs(changes), accel * avg_time)
        return self.last_speed
