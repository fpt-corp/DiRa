#!/bin/bash
git clone https://github.com/richardstechnotes/RTIMULib2.git
cd RTIMULib2/Linux
mkdir build
cd build
cmake ..
make -j4
sudo make install
sudo ldconfig
sudo apt-get install i2c-tools
sudo apt-get install libi2c-dev
