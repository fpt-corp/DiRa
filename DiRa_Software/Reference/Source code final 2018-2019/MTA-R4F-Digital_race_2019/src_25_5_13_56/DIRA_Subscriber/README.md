# Mqtt Subscriber
Source code subscriber messages from the server mqtt

id: Order to draw your team, default: 1.
server ip: 192.168.1.123, default: localhost.

Script pub mqtt for test ROS: /scripts/server_publish_demo.py

Script receive mqtt -> publish ROS topic: /route
  * data: route, ex: 1-2-3-4-5
  
Run script: rosrun subscriber publish.py

## Install
>  pip install paho-mqtt
