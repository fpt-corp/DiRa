## DiRa PCA9685 Controller
Make sure your car engine started using EZ button on the ESC and battery sercure under the car's base.
Use the Terminal to run the following command:
```
  roslaunch dira_pca9685_controller run_car.launch
```
## Topics
---
### Subscriber
```
/set_speed (Float) [-100 : 100]
Control the speed of the car by percent
```
* Positive values make car move forward, negative values make car move backward.
* Small percent close to zero might not make the car move base on friction.
* 0 make the car brake when running, stand still when not moving.
* Change car direction (From positive to negative or vise versa) make the car brake and go to the other direction
```
/set_angle (Float) [-60 : 60]
Control the angle of the car by angle degree
```
* 0 car's angle will be in straight forward positon
* Negative and positive values will lean the car angle to the corresponding left or right angle.
