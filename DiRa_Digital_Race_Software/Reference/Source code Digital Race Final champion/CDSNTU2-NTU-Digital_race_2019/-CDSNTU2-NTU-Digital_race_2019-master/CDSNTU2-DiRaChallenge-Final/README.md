# This is our open source code for Digital Race Challenge 2018-2019 in Final Round.
* In order to run this code, you just need to clone this repo to your Ubuntu on Jetson TX2 Board. Remember to replace package team515 beacause we just create two package same name.

* For Round 1. 
> roslaunch team515 auto.launch
* For Round 2.
> roslaunch team515 team515.launch

* To get data you need F710 Gamepad and plug it into Jetson TX2. Run command
> roslaunch team515 joy.launch

 It's will record data to /home/nvidia/Data with name following format : timestamp_speed_steer.jpg
