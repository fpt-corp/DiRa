## Setup Jetpack for Jetson TX2 (perform on host machine)

Checkout [this](https://github.com/fpt-corp/DiRa/blob/master/DiRa_Software/Jetson_TX2/Document/Setup_Flash_Jetson_TX2.txt) instruction to flash the Jetson TX2 with the latest image supported


## Install basic dependencies
Installing all the necessary dependencies that will help your code compile

```bash
sudo ./setup.sh
```

## Install Ros


Choose the suitable version of ROS on the TX2: 
	+ Kinetic for Ubuntu 16.04
	+ Melodic for Ubuntu 18.04
and install following the instruction on the ROS official wiki: wiki.ros.org/<distro>/installation/Ubuntu
   *Note: During the installation process, errors may occured. This part will cover some possible ones.
	When using the command $sudo apt install ros-<distro>-desktop-full, get error:
```
	  E: Could not get lock /var/lib/dpkg/lock-frontend - open(11: Resource temporarily unavailable)
	  E: Unable to aquire the dpkg frontend lock
```
	
This is due another process getting stuck or currently running
=> Solution: 
Check running processes containing "apt" with: 
	```$ps aux | grep [a]pt```
Wait a bit and check again if the process is still running or not. 
It could just be running and not stuck!
If the process is stuck, try killing it with 		
	```$sudo kill -9 <process_id>``` 
(The process ID is the first number on the output)
## Setup the catkin workspace: 
Use the following commands to create
```
mkdir -p ~/catkin_ws/src
```
To compile the workspace, we should source the ROS environment to get access to ROS functions
```
    $ source /opt/ros/<distro>/setup.bash
```
Initialize a new catkin workspace
```
    $ cd ~/catkin_ws/src
    $ catkin_init_workspace
```
Build the workspace
```
    $ cd ~/catkin_ws
    $ catkin_make
```
*Note: catkin_ws can be any workspace name you want
Source the new setup.*sh file
```		
    $ source devel/setup.bash
```
To source the setup.*sh file everytime we start a new bash session, we use 
```
    $ echo "source ~/catkin_ws/devel/setup.bash" >> ~/.bashrc
    $ source ~/.bashrc
```	
To ensure the workspace i properly overlayed by the setup script
```		
    $echo $ROS_PACKAGE_PATH /home/<user_name>/catkin_ws/src:/opt/ros/<distro>/share
```	  
this should return the same path

## Orbbec Astra
Orbbec Astra is the package that handle the connection between the device and the Orbbec Camera. You can follow the official installation guide [here](https://github.com/orbbec/ros_astra_camera)

## RTIMULib2
RTIMULib2 is the library that helps you calibrate and acquire data from the IMU sensor mounted on Jetson TX2. 

```bash
sudo ./rtimulib2.sh
```
## DiRa APIs
DiRa project also provide you with the necessary packages to help you control the rover better:
 hal: provide the node to control the proximity sensor at the front of the car, as well as the LCD screen and the four buttons plus a led indicator
 car_control: provide the node to control the car's speed and steering angle through the ESC
 mpu_9250: provide the node to acquire data from the IMU sensor

```bash
svn checkout https://github.com/fpt-corp/DriverlessCar_DR_ROS/trunk/core

sudo cp -a core/* ~/catkin_ws/src/

cd ~/catkin_ws/

catkin_make

cd ~/catkin_ws/devel/lib/car_controller/

sudo chown root:root car_controller_node
sudo chmod a+rx car_controller_node
sudo chmod u+s car_controller_node


cd ~/catkin_ws/devel/lib/hal/

sudo chown root:root hal_node
sudo chmod a+rx hal_node
sudo chmod u+s hal_node


cd ~/catkin_ws/devel/lib/mpu_9250/

sudo chown root:root mpu_9250_node
sudo chmod a+rx mpu_9250_node
sudo chmod u+s mpu_9250_node

```


