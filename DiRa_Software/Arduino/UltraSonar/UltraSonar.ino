#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <ros.h>
#include <std_msgs/Float32MultiArray.h>
#include <string.h>


ros::NodeHandle_<ArduinoHardware, 10, 15, 128, 256> nh;
//ros::NodeHandle nh;
std_msgs::Float32MultiArray message;
ros::Publisher distance1("sensors/sonar_data",&message);
//ros::Publisher distance1("sensors/sonar_data1",&message[0]);
//ros::Publisher distance2("sensors/sonar_data2",&message[1]);
//ros::Publisher distance3("sensors/sonar_data3",&message[2]);
//ros::Publisher distance4("sensors/sonar_data4",&message[3]);


float getDistance(int trig,int echo){
    pinMode(trig,OUTPUT);
    digitalWrite(trig,LOW);
    delayMicroseconds(2);
    digitalWrite(trig,HIGH);
    delayMicroseconds(10);
    digitalWrite(trig,LOW);
    pinMode(echo, INPUT);
    return pulseIn(echo,HIGH,30000)/58.0;
}



void setup(){
    nh.initNode();
    nh.advertise(distance1);
    //nh.advertise(distance2);
    //nh.advertise(distance3);
    //nh.advertise(distance4);
    
}

void loop(){
    message.data[0] = getDistance(4,5);
    message.data[1] = getDistance(6,7);
    message.data[2] = getDistance(8,9);
    message.data[3] = getDistance(15,16);

    distance1.publish(&message);
    //distance2.publish(&message[1]);
    //distance3.publish(&message[2]);
    //distance4.publish(&message[3]);
    nh.spinOnce();
    delay(500);
}
