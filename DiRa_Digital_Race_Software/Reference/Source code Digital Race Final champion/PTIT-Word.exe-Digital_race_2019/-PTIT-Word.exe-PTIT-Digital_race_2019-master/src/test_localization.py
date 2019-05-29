#!/usr/bin/env python
from localization import CarPose, Position, CheckPoint
from param import Param
import rospy

import tf
import geometry_msgs.msg

param = Param()

# 1->2->3
list_state = [{"start": '1',  # 1->2
               '1': 's',
               '2': 'l',
               '3': 's'},
              {"start": '2', # 2->3
               '1': 's',
               '2': 'l',
               '3': 'r'},
              
              {"start": '3',  # 2->3
               '1': 's',
               '2': 's',
               '3': 'r'}]

# init car_pose
car_pose = CarPose(param.checkpoint1)
car_pose.update_list_state(list_state)

def update_pos(pos):
    car_pose.update_position(pos)
    print("car_pose", car_pose.position.x, car_pose.position.y, car_pose.position.z)
    print("list_state", len(car_pose.list_state))
    print("current_state_start", car_pose.current_state["start"])
    print("check_point", car_pose.last_checkpoint.name)
    print("turn", car_pose.turn)
    print("------------------")

if __name__ == '__main__':
    rospy.init_node('translation_tf_listener')

    listener = tf.TransformListener()

    rate = rospy.Rate(10.0)

    while not rospy.is_shutdown():
        try:
            (trans,rot) = listener.lookupTransform('/camera_link', '/camera_pose', rospy.Time(0))

            # print("position: ", trans[0], trans[1], trans[2])
            x, y, z = trans[0]*1000, trans[1]*1000, trans[2]*1000
            update_pos(Position(x, y, z))
        except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
            continue

        rate.sleep()



