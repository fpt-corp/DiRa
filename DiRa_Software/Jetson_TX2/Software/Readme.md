# DiRa source code documentations

This is the documentations for the DiRa Modules. They will guide you through implementing and understanding these modules


![alt text][note] Please contact us by opening an issue or emailing if you have any questions about the instructions, parts, or any part of the build process. My email: manhlinh.nguyendinh@gmail.com.

## Getting Started

To implement the modules provided in this document, you need to have understanding of Python programming. If you would like to further understand and modify the code to your liking, you also need to understand C++, Python's deeplearning framework Tensorflow and Keras. 

## Included Modules:

### [DiRa_Astra_Camera](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_Astra_Camera)

Orbbec Astra's camera handling module.

### [DiRa_Beacon](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_Beacon]

DiRa's beacon signal handling module

### [DiRa_Controller](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_Controller)

DiRa car's controller module

### [DiRa_GPIO_Input](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_FPIO_Input)

DiRa car's input module, which enable the Jetson board to get sensor data from GPIO input signals 

### [DiRa_GPIO_Output](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_GPIO_Ouput)

DiRa car's output module, which enable the Jetson board to send command to the car's ESC through GPIO ouput signals

### [DiRa_Lane_Detection](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_Lane_Detection)

DiRa car's lane detection module, which helps the controller "see" the road in front of the car from the image input of the camera

### [DiRa_Localization](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_Localization)

DiRa car's localization module, which helps the car know it's current estimated position in a map setting

### [DiRa_Object_Detection](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_Object_Detection)

DiRa car's object detection module, which enable the controller to identify obstacles in the field of view

### [DiRa_Traffic_Sign](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_Traffic_Sign)

DiRa car's traffic sign detection module, which helps the controller detect traffic sign in the field of view and alter course accordingly

### [DiRa_Lidar](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_Lidar)

DiRa 's Lidar handling module

### [DiRa_Sonar](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_Sonar)

DiRa's Sonar handling module

### [DiRa_Library](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_Library)

DiRa's programing libraries, which helps develop the system core and other functions.

### [DiRa_Manual](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_Manual)

DiRa's manual control module, enabling you to control your car with a keyboard or joystick.
