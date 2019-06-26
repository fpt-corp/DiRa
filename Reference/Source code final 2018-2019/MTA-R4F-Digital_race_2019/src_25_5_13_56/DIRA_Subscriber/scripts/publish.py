#!/usr/bin/env python

import rospy
import json
from std_msgs.msg import String
import paho.mqtt.client as mqtt

#broker="192.168.1.123"  # ip server
broker="192.168.1.124"
team_id = 3

m_client = mqtt.Client()

#init ros node
pub = rospy.Publisher('route', String, queue_size=10)
rospy.init_node('publish', anonymous=True)

def publish(data):
	print(data)
	pub.publish(data)

def on_connect(client, userdata, flags, rc):
	m_client.subscribe("route")

def on_message(client, userdata, message):
	datastore = json.loads(str(message.payload))
	if datastore["team"] == team_id:
		publish('-'.join(str(x) for x in datastore["route"]))

def init():	
	m_client.on_message=on_message
	m_client.on_connect = on_connect
	m_client.connect(broker, 1883, 60)
	print("connect to broker")
	m_client.loop_forever()

if __name__ == '__main__':
    init()
