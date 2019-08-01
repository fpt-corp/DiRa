#!/bin/bash
sudo apt update
sudo apt upgrade
sudo apt install python-pip
sudo apt install libhdf5-serial-dev hdf5-tools
sudo apt-get install openjdk-8-jdk 
sudo apt-get install libblas-dev liblapack-dev
sudo apt-get install libfreetype6-dev pkg-config libpng12-dev

pip install pip==9.0.1
pip install numpy
pip install panda

pip install cython
pip install scipy==1.2.0
pip install matplotlib
pip install 'scikit-image<0.15'
 

sudo pip install -U numpy enum34 grpcio absl-py py-cpuinfo psutil portpicker six mock requests gast h5py astor termcolor protobuf keras-applications keras-preprocessing wrapt google-pasta

pip install keras --user





