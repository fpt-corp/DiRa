cmake_minimum_required(VERSION 2.8.3)
project(beacon_rssi)

find_package(catkin REQUIRED COMPONENTS
  actionlib
  actionlib_msgs
  roscpp
  std_msgs
  message_generation
)

add_message_files(
  FILES
  beacon.msg
  
)
generate_messages(
  DEPENDENCIES
  actionlib_msgs
  std_msgs
)

catkin_package(
  INCLUDE_DIRS include
#  LIBRARIES beacon_rssi
  CATKIN_DEPENDS actionlib actionlib_msgs roscpp std_msgs
#  DEPENDS system_lib
)


include_directories(
 include
  ${catkin_INCLUDE_DIRS}
)
add_library(${PROJECT_NAME}
  src/SimpleKalmanFilter.cpp
)
add_executable(beacon_rssi_publisher src/beacon_rssi_publisher.cpp)
add_executable(beacon_rssi_subscriber src/beacon_rssi_subscriber.cpp)

add_dependencies(beacon_rssi_publisher beacon_rssi_generate_messages_cpp)
add_dependencies(beacon_rssi_subscriber beacon_rssi_generate_messages_cpp)

target_link_libraries(beacon_rssi_publisher ${catkin_LIBRARIES} ${PROJECT_NAME} bluetooth)
target_link_libraries(beacon_rssi_subscriber ${catkin_LIBRARIES})


