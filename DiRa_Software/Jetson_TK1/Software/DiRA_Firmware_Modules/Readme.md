___
# DiRa Firmware 
___
> Ros Nodes for basic car functionalities
___
## TK1 HGP402 PCA9685 Controller Node
___
> Control the speed and turn of the HG-P402 with Jetson TK1 via pca9685 <br/>
### Ubuntu version:
> ubuntu 14.04 <br/>
> ubuntu 16.04

### Car Model:
HGP402
>Link: https://www.amazon.in/KKY-Without-Electronic-Roadster-Climbing/dp/B07L3P3QRC

### Mod ESC from Strom Racer
>Link: http://www.stormracer.com/sanpham/mo-to-dieu-toc/dieu-toc-esc-for-car/quicrun-brushed-esc-wp-1040-waterproof_12020027.product
___
## MPU9685 Node
___
> Raw data from Sparkfun's MPU9685 <br/>
> Relative angle of the car

___
## LCD Node
___
> Control LCD 16 x 4 I2C Display <br/>
Can write in the format: ``[row]:[col]:[data]``
___
## GPIO Node
___
> Basic GPIO functionality
- push button (4 button)
- proximity sensor (2 sensor's slot)
- LED indicator (1 LED)
## Ackermann Dirive Teleop
___
> Control the car remotely using:
- keyboard (keyop.py)
- gamepad (joyop.py)
- required package: <joy joy_node>
