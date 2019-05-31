import numpy as np
import time
def Fuzzy(error):
    sign = np.sign(error)
    error =abs(error)
    k1 = 0.35
    x = 50
    k2 = 0.55
    angle = 0
    if error<x:
        angle =  k1*error
    else:
        angle = (error-x)*k2+k1*x
    if angle>60:
        angle = 60
    return -angle*sign

error_arr = np.zeros(5)
t = time.time()
def PID(error, p= 0.43, i =0, d = 0.02):
    global error_arr, t
    error_arr[1:] = error_arr[0:-1]
    error_arr[0] = error
    P = error*p
    delta_t = time.time() - t
    t = time.time()
    D = (error-error_arr[1])/delta_t*d
    I = np.sum(error_arr)*delta_t*i
    angle = P + I + D
    if abs(angle)>60:
	angle = np.sign(angle)*60
    return -int(angle)
