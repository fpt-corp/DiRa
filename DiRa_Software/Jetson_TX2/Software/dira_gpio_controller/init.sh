#!/bin/bash

sudo groupadd -f -r gpio
sudo usermod -a -G gpio ubuntu
cp scripts/99-gpio.rules /etc/udev/rules.d/
sudo udevadm control --reload-rules && sudo udevadm trigger
