# GUIDELINE for Simulator

## Install

Download link: http://bit.ly/cds-simulator-2019

## Software manuals

Run simualator.x86-64

In open program interface

    Enter the team name:
    Enter the IP of the server running ROS_MASTER according to the structure ws://xxx.xxx.xxx.xxx:
              xxx.xxx.xxx.xxx: IP of the server running ROS_MASTER
              pppp: Port of the server running ROS_MASTER
    
![alt text](https://github.com/fpt-corp/DiRa/blob/master/Image_Resource/DiRa_Simulator_1.png)

    Once connected, the program will count down to 2 minutes and
    Transfer images from the captured camera to ROS MASTER. With the resulting image,
    Teams must pass back the speed and the steering angle with the topic defined as follows:
    
/teamName/set_speed: Topics published from ROS_node are defined as real numbers (Float32). (default is
/team1/set_speed, if you enter another team name, you need to edit the topic in /lane_detect/src/main.cpp according to the team name entered in the app)

/teamName/set_angle: Topics published from ROS_node are defined as real numbers (Float32). (default is
/team1/set_speed, if you enter another team name, you need to edit the topic in /lane_detect/src/main.cpp according to the team name entered in the app)

/teamName/set_camera_angle: Topics published from ROS_node are defined as real numbers (Float32).

/teamName/camera/rgb/compressed: Topic is used to subcribe the rgb image obtained on vehicle. The resulting image is a "img" standard image.(default is
/team1/camera/rgb, if you enter another team name, you need to edit the topic in /lane_detect/src/main.cpp according to the team name entered in the app)

/teamName/camera/depth/compressed: Topic is used to subcribe the rgb image obtained on vehicle. The resulting image is a "img" standard image.(default is
/team1/camera/depth, if you enter another team name, you need to edit the topic in /lane_detect/src/main.cpp according to the team name entered in the app)

## Connect the ROS Master server to the simulation software

B1: Download the basic package at https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/ROS_Package_example

video_stream: Create websocket to connect to the simulator software

lane_detect: image processing algorithm of the organizer

B2: Run sample program

Create websocket to connect to the simulator software

`$ roslaunch video_stream video_stream.launch`

Test the images received from the simulator software

`$ rosrun video_stream video_stream _image_transport:=compressed` 

Test sample code

`$ rosrun lane_detect lane_detect _image_transport:=compressed`
