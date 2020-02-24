#!/usr/bin/env python

import rospy
from std_msgs.msg import Bool
import RPi.GPIO as GPIO
import time

# Pin Definitons:
led_pin = 18  	 # Board pin 18
but_pin_1 = 35   # Board pin 12
but_pin_2 = 38
but_pin_3 = 40
but_pin_4 = 12
sen_pin_1 = 29
sen_pin_2 = 7  

def setLed(data):
    if(data.data == True):
        GPIO.output(led_pin, GPIO.HIGH)
    elif(data.data == False):
        GPIO.output(led_pin, GPIO.LOW)


def main():
    # Pin Setup:
    GPIO.setwarnings(False)
    GPIO.setmode(GPIO.BOARD)  # BOARD pin-numbering scheme
    GPIO.setup(led_pin, GPIO.OUT)  # LED pin set as output
    GPIO.setup(but_pin_1, GPIO.IN)  # button pin set as input
    GPIO.setup(but_pin_2, GPIO.IN)
    GPIO.setup(but_pin_3, GPIO.IN)
    GPIO.setup(but_pin_4, GPIO.IN)
    GPIO.setup(sen_pin_1, GPIO.IN)
    GPIO.setup(sen_pin_2, GPIO.IN)
    # Init ROS node
    rospy.init_node('gpio', anonymous=True)
    rate = rospy.Rate(10)

    # Subscribe to led_status topic
    rospy.Subscriber('led_status', Bool, setLed)
    button1 = rospy.Publisher('bt1_status', Bool, queue_size=10)
    button2 = rospy.Publisher('bt2_status', Bool, queue_size=10)
    button3 = rospy.Publisher('bt3_status', Bool, queue_size=10)
    button4 = rospy.Publisher('bt4_status', Bool, queue_size=10)
    sensor1 = rospy.Publisher('ss1_status', Bool, queue_size=10)
    sensor2 = rospy.Publisher('ss2_status', Bool, queue_size=10)
    
    while not rospy.is_shutdown():
        button1.publish(GPIO.input(but_pin_1))
        button2.publish(GPIO.input(but_pin_2))
        button3.publish(GPIO.input(but_pin_3))
        button4.publish(GPIO.input(but_pin_4))
        sensor1.publish(GPIO.input(sen_pin_1))
        sensor2.publish(GPIO.input(sen_pin_2))
        rate.sleep()
    GPIO.cleanup()
if __name__ == '__main__':
    main()
