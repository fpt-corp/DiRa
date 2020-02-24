# DiRa GPIO Controller
## A package made for Jetson TX2 DIRA shield's GPIO
### (Tested on Jetpack 4.3)
# How to run this package:
```
Step0: Open Terminal in the "dira_gpio_controller" package
Step1: Add new udev rules and group permission:
sudo bash init.sh
Step2: cd ~/catkin_ws
Step3: catkin_make
Step4: rosrun dira_gpio_controller dira_gpio_controller.py
Step5: subcribe/publish manually below topic using rostopic echo/pub for testing
Step6: done!
```

# Topics
## Subscribers
---
### LED
```
led_status (Bool)
True - LED turn on
False - LED turn off
```

## Publishers
---
### Buttons
```
bt1_status (Bool) - button 1 pressed or not
..
..
bt4_status (Bool) - button 4
```
### Sensors
```
ss1_status (Bool) - proximity sensor slot 1
ss2_status (Bool) - proximity sensor slot 2
```