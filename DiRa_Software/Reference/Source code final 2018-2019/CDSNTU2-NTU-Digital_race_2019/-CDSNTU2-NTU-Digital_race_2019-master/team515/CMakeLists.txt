cmake_minimum_required(VERSION 2.8.3)
project(team515)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  cv_bridge
  image_transport
  roscpp
  sensor_msgs
)

set(OpenCV_STATIC OFF)
find_package(  OpenCV REQUIRED )
include_directories(${OpenCV_INCLUDE_DIRS})

catkin_package(
  INCLUDE_DIRS include
  LIBRARIES team515
  CATKIN_DEPENDS roscpp
  DEPENDS system_lib
)

include_directories(
  include
  ${catkin_INCLUDE_DIRS}
)

add_library(team515
  src/detectlane.cpp
  src/detectsign.cpp
  src/carcontrol.cpp
  #src/detectobject.cpp	
)

add_executable(team515_node src/main.cpp)

add_dependencies(team515_node ${catkin_EXPORTED_TARGETS})

target_link_libraries(
  team515_node
  team515
  ${catkin_LIBRARIES}
  ${OpenCV_LIBS}
  ${Boost_LIBRARIES}
)


