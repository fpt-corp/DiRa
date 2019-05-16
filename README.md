# DiRa

![alt text](https://github.com/fpt-corp/DiRa/blob/master/Image_Resource/Digital_Race_Car_1.jpg "Platform image")


[DiRa Platform Website](link) - update soon

[DiRa Youtube Channel](link) - update soon

Research Pages AutoRally is associated with:
  * link - update soon

## Contributing

We welcome bug fixes, ehancements, new features, and feedback at: https://github.com/fpt-corp/DiRa/issues or email: manhlinh.nguyendinh@gmail.com

Please contact to [me](manhlinh.nguyendinh@gmail.com) before submit pull requests 
We use Gitflow, so master branch is reserved for releases.

## Setup Instructions

### Contents
1.   [Dira Car Platform Instructions](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Digital_Race_Platform_Instruction) - setup, construct, operate and assembley a complete physical Dira platform.
2.   [DiRa Car software] - software for DiRa research platform 

	2.1. [Install Prerequisites](#1-install-prerequisites)

	2.2. [Clone repository](#2-clone-or-fork-repositories)

	2.3. [Install DiRa ROS Dependencies](#3-install-dira-ros-dependencies)

	2.4. [Compilation/Running](#4-compilation-running)

	2.5. [Generate Documentation](#5-generate-documentation)

	2.6. [Test Setup in Simulation](#6-test-setup-in-simulation)

	2.7. [Autonomous Driving in Simulation](#7-autonomous-driving-in-simulation)

### 2.1. Install Prerequisites
#### 2.1.1 Computer (Desktop/Laptop)
1. __Install [Ubuntu 16.04 64-bit](https://www.ubuntu.com/download/desktop)__
2. __Install required packages__
    
* ```sudo apt-get install git doxygen openssh-server libusb-dev texinfo```

* ```sudo apt-get install libfontconfig1-dev libdbus-1-dev libfreetype6-dev libudev-dev libicu-dev libsqlite3-dev libxslt1-dev libssl-dev libasound2-dev libavcodec-dev libavformat-dev libswscale-dev libgles2-mesa-dev libxcb-icccm4-dev libxcb-image0-dev libxcb-keysyms1-dev libxcb-xinerama0-dev libprotobuf-dev libleveldb-dev libsnappy-dev libhdf5-serial-dev protobuf-compiler libopenblas-dev liblapack-dev libopenblas-dev liblapack-dev libgflags-dev libgoogle-glog-dev liblmdb-dev cmake cmake-gui gfortran liblapack-dev libblas-dev libatlas-base-dev libarpack2-dev libarpack++2-dev```
* If you code with python you need to install ```sudo apt install python-rosinstall python-rosinstall-generator python-wstool build-essential```

3. __Install [OpenCV 3.4.3](https://github.com/opencv/opencv/archive/3.4.3.zip)__
4. __Install [ROS kinetic](http://www.ros.org/install/) ros-kinetic-desktop-full__
5. __Install Cuda_ 9.0 and cuDNN__

* Please follow [Guideline](https://github.com/fpt-corp/DiRa/blob/master/DiRa_Digital_Race_Software/Jetson_TX2/Document/cuda_installation_on_ubuntu_18.md)

#### 2.1.2 Embedded computer (Jetson TX2)
1. __Install [ROS kinetic](https://github.com/jetsonhacks/installROSTX2)__

```
	git clone https://github.com/jetsonhacks/installROSTX2
	cd installROSTX2
	./installROS.sh
	./setupCatkinWorkspace.sh
```
2. __Install driver for [astra camera](http://wiki.ros.org/astra_camera)__
```
	cd  /etc/udev/rules.d/
	sudo gedit 56-orbbec.rules
```

* Copy the following lines:

```
	SUBSYSTEM=="usb", ATTR{idProduct}=="0400", ATTR{idVendor}=="2bc5", MODE:="0666", OWNER:="root", GROUP:="video"
	SUBSYSTEM=="usb", ATTR{idProduct}=="0401", ATTR{idVendor}=="2bc5", MODE:="0666", OWNER:="root", GROUP:="video"
	SUBSYSTEM=="usb", ATTR{idProduct}=="0402", ATTR{idVendor}=="2bc5", MODE:="0666", OWNER:="root", GROUP:="video"
	SUBSYSTEM=="usb", ATTR{idProduct}=="0403", ATTR{idVendor}=="2bc5", MODE:="0666", OWNER:="root", GROUP:="video"
	SUBSYSTEM=="usb", ATTR{idProduct}=="0404", ATTR{idVendor}=="2bc5", MODE:="0666", OWNER:="root", GROUP:="video"
	SUBSYSTEM=="usb", ATTR{idProduct}=="0405", ATTR{idVendor}=="2bc5", MODE:="0666", OWNER:="root", GROUP:="video"
	SUBSYSTEM=="usb", ATTR{idProduct}=="0406", ATTR{idVendor}=="2bc5", MODE:="0666", OWNER:="root", GROUP:="video"
	SUBSYSTEM=="usb", ATTR{idProduct}=="0407", ATTR{idVendor}=="2bc5", MODE:="0666", OWNER:="root", GROUP:="video"
	SUBSYSTEM=="usb", ATTR{idProduct}=="0408", ATTR{idVendor}=="2bc5", MODE:="0666", OWNER:="root", GROUP:="video"
	SUBSYSTEM=="usb", ATTR{idProduct}=="0409", ATTR{idVendor}=="2bc5", MODE:="0666", OWNER:="root", GROUP:="video"
	SUBSYSTEM=="usb", ATTR{idProduct}=="040a", ATTR{idVendor}=="2bc5", MODE:="0666", OWNER:="root", GROUP:="video"
```
* Save and exit

```
sudo service udev reload
sudo service udev restart
```

* Unplug the camera and plug it again.

```
	cd ~/catkin_ws/src
	git clone https://github.com/orbbec/ros_astra_camera
	cd ..
	catkin_make --pkg astra_camera
	catkin_make --pkg astra_camera -DFILTER=OFF
	roscd astra_camera && ./scripts/create_udev_rules
	cd src
	git clone https://github.com/orbbec/ros_astra_launch
	cd ..
	catkin_make
```

3. __Install some package for control car__
```
	sudo apt-get install libi2c-dev
	cd ~/catkin_ws/src
```
* Package for control button and sensor
```
	git clone https://github.com/ducvietmta/hal_ros
```
* Package for control mpu_9250
```
	git clone https://github.com/ducvietmta/mpu_9250
```
* Package for control car (steer and speed)
```
	https://github.com/ducvietmta/car_controller
```
* Compile

	```
	cd ..
	catkin_make
	```
4. __Driver car and test__

	```
	cd ~/catkin_ws/src
	git clone https://github.com/totai02/lane_detect
	cd ..
	catkin_make
	```

* Run ROS node:

	* Open a new terminal:

		```roslaunch astra_launch astra.launch```

	* Open a new terminal:

		```rosrun hal hal_node```

	* Open a new terminal:

		```rosrun carcontroller carcontroller_node```

	* Open a new terminal:

		```rosrun lanedetect lanedetect```
