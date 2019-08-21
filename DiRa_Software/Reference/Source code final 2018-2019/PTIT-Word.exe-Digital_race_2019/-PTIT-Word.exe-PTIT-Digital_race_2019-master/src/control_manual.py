import pygame
import time
import rospy
from std_msgs.msg import Float32

rospy.init_node('team105', anonymous=True)
#speed_pub = rospy.Publisher("/set_speed_car_api", Float32, queue_size=1)
#steerAngle_pub = rospy.Publisher("/set_steer_car_api", Float32, queue_size=1)
rospy.Rate(10)

pygame.init()
# Initialize the joysticks
pygame.joystick.init()

# -------- Main Program Loop -----------
start = False
time_finish = time.time()
while True:
    # EVENT PROCESSING STEP
    for event in pygame.event.get():  # User did something
        if event.type == pygame.QUIT:  # If user clicked close
            done = True  # Flag that we are done so we exit this loop

    joystick = pygame.joystick.Joystick(0)
    joystick.init()

    axis_h = joystick.get_axis(1)
    axis_v = joystick.get_axis(2)
    s = joystick.get_button(2)
    turn_off = joystick.get_button(3)

    timer = time.time() - time_finish
    if timer > 1 and s == 1:
        start = not start
        time_finish = time.time()

    if turn_off == 1:
        break

    # pre-process
    axis_h = max(0, axis_h * (-10))
    axis_v *= -100

    print("H {}".format(axis_h) + "\tV {}".format(axis_v) + "\tStart {}".format(start))

    # TOPIC....
#    if start:
#        speed_pub.publish(axis_h)
#        steerAngle_pub.publish(axis_v)
