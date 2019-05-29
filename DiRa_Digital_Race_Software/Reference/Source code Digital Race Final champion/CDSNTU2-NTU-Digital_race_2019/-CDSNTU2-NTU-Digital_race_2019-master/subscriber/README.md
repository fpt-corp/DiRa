# Mqtt Subscriber
Mã nguồn subscriber bản tin lộ trình từ server

id: thứ tự bốc thăm của đội khi thi đấu, mặc định là 1,
server ip: 192.168.1.123, mặc định là localhost.

Script pub mqtt để test ROS: /scripts/server_publish_demo.py

Script nhận mqtt -> publish ROS topic: /route
  * data: lộ trình, vd: 1-2-3-4-5
  
Run script: rosrun subscriber publish.py

## Cài đặt
>  pip install paho-mqtt
