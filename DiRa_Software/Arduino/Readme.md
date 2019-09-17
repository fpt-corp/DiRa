# DiRa Arduino IDE Setup Instructions


The documents contained in this repository provide all of the information required to setup, install and operate the Arduino UNO board that will assist with collecting informations from the sonar sensors and additional sensors that will be included in future versions. Only perform the instructions in this document once you have finished setting up the Jetson boards and the DiRa software

## Getting Started with Arduino IDE

Look though the documentation and [Download](https://www.arduino.cc/en/Main/Software) the latest release for ARM 64 architecture. You will most like download a tar.gz file, in which case you can use the tar command to extract that file.

```
sudo tar xzvf <arduino-ide-version>.tar.gz

cd <arduino-ide-version>
 
sudo chmod +x install.sh

sudo ./install.sh
```

After this, your home/<user-name>/Arduino should exist. Copy the source code provided in this repository folder to that folder. After this, you will need to perform the [ros_lib installation guide](https://github.com/fpt-corp/DiRa/tree/master/DiRa_Software/Jetson_TX2/Software/DiRa_Sensors/DiRa_Sonar/rosserial_arduino/src/rosserial_arduino) in order to compile your code with out error.

## Arduino guide

For a detail instruction on how to compile codes on Arduino, please checkout [this](https://www.arduino.cc/en/Guide/ArduinoUno) official guid from the Arduino Homepage. 
