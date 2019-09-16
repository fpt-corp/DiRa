#include <Arduino.h>
#include <Wire.h>
#include <SoftwareSerial.h>
#include <ros.h>
#include <std_msgs/Float32MultiArray.h>
#include <string.h>
#include <Adafruit_NeoPixel.h>

ros::NodeHandle nh;

std_msgs::Float32MultiArray message;
ros::Publisher distance("sensors/sonar_data",&message);
double angle_rad = PI/180.0;
double angle_deg = 180.0/PI;
char data[50];
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(17, 13, NEO_GRB + NEO_KHZ800);
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
    nh.advertise(distance);
    pixels.begin();
    pixels.clear();
    pixels.show();
    Serial.begin(57600);
    
}

void loop(){
    message.data[0] = getDistance(2,3);
    message.data[1] = getDistance(4,5);
    message.data[2] = getDistance(6,7);
    message.data[3] = getDistance(8,9);
    message.data[4] = getDistance(10,11);
    distance.publish(&message);
    nh.spinOnce();
    Serial.println(getDistance(2,3));
    _delay(1);    
    _loop();
}

void _delay(float seconds){
    long endTime = millis() + seconds * 1000;
    while(millis() < endTime)_loop();
}

void _loop(){
    
}
