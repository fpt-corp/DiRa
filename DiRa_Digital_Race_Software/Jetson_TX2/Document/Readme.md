#Install basic dependencies
Installing all the necessary dependencies that will help your code compile

```bash
sudo ./setup.sh
```

# Orbbec Astra
Orbbec Astra is the package that handle the connection between the device and the Orbbec Camera. You can follow the official installation guide [here](https://github.com/orbbec/ros_astra_camera)

# RTIMULib2
RTIMULib2 is the library that helps you calibrate and acquire data from the IMU sensor mounted on Jetson TX2. 

```bash
sudo ./rtimulib2.sh
```
# DiRa APIs
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


