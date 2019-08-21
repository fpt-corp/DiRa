# **Introduction**

***
The DiRa_Localization is built for check car localization with BLE(Beacon). All configuration and code run is broken up into two packages: `Beacon_RSSI` and `Beacon_Scan`.

**_Important_**: _Before use DiRa_Localization please install BlueZ package_

## **1. Beacon_RSSI**

***
Sample for scan RSSI Beacon
### scan_one_beacon.cpp
Scan RSSI one Beacon
### scan_all.cpp
Scan RSSI all beacon around.
### scan_kalman.cpp
Scan RSSI only one Beacon you chose with Kalman filter

## **2. Beacon_Scan**

***
A ROS package for Beacon Scan.



# **Running**

***
Use terminal and flow steps below

## **1. Beacon_RSSI**

***
### scan_one_beacon.cpp
* Step 1: `gcc scan_one_beacon.cpp -lbluetooth -o scan_one_beacon`
* Step 2: `sudo setcap 'cap_net_raw,cap_net_admin+eip' scan_one_beacon`
* Step 3: `./scan_one_beacon`
### scan_all.cpp
* Step 1: `gcc scan_all.cpp -lbluetooth -o scan_all`
* Step 2: `sudo setcap 'cap_net_raw,cap_net_admin+eip' scan_all `
* Step 3: `./scan_all` 
### scan_kalman.cpp
* Step 1: `gcc scan_kalman.cpp -lbluetooth -o scan_kalman`
* Step 2: `sudo setcap 'cap_net_raw,cap_net_admin+eip' scan_kalman`
* Step 3: `./scan_kalman`
## **2. Beacon_Scan**

***

Subscribe topic `/beacon` for getting information RSSI and Distance with Kalman filter.

You can combine catkin_make and setcap with 5 step:
* Step 1: `cd catkin_ws/`
* Step 2: `gedit ~/.bashrc`
* Step 3: copy line code in the end of file : 
`alias build_setcap = ' catkin_make -DCATKIN_WHITELIST_PACKAGES = "beacon_rssi" && sudo setcap 'cap_net_raw,cap_net_admin+eip' ~/catkin_ws/devel/lib/beacon_rssi/beacon_rssi_publisher '`
* Step 4: `source ~/.bashrc`
* Step 5: `build_setcap`

# **Config**
**_Update soon_**
