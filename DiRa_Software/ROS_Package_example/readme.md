# ROS_Package_example
## I. Cài đặt
1.Trước tiên cần cài đặt Ubuntu 16.04 hoặc mới hơn.

2.Lựa chọn 1 trong 2 phiên bản ROS sau:

  Cài đặt ROS Lunar:
  http://wiki.ros.org/lunar/Installation/Ubuntu
  
  Cài đặt ROS Melodic:
  http://wiki.ros.org/melodic/Installation/Ubuntu
  
  Chú ý: cần cài đặt bản đầy đủ. Ví dụ với ROS Lunar thì cài đặt bản đầy đủ như sau: 
  
  $sudo apt-get install ros-lunar-desktop-full
  
  Hoàn Thiện đầy đủ các bước từ mục 1.1 đến 1.7
  
3. Tạo ROS Workspace

  `$ mkdir -p ~/catkin_ws/src`
  
  `$ cd ~/catkin_ws/`
  
  `$ catkin_make`
  
  `$ echo "source ~/catkin_ws/devel/setup.bash" >> ~/.bashrc`

  `$ source ~/.bashrc`
  
4. Cài đặt rosbridge-suite

`$ sudo apt-get install ros-lunar-rosbridge-server`