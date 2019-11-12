cmake_minimum_required(VERSION 2.8.3)
project(dira_pca8266_controller)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS roscpp std_msgs)

catkin_package(INCLUDE_DIRS include LIBRARIES ${PROJECT_NAME} CATKIN_DEPENDS roscpp std_msgs ackermann_msgs
DEPENDS 
#system_lib
)

###########
## Build ##
###########

## Specify additional locations of header files
## Your package locations should be listed before other locations
include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  /usr/include/i2c/
  /usr/lib/x86_64-linux-gnu/
)

## Declare a C++ library

add_library(controller src/dira_pca8266_controller.cpp)

add_library(pca9685 src/JHPWMPCA9685.cpp)


#add_library(i2c_stuff /usr/lib/x86_64-linux-gnu/libi2c.so)

target_link_libraries(pca9685 -li2c)
target_link_libraries(controller pca9685)

add_executable(${PROJECT_NAME}_node src/dira_pca8266_subscriber.cpp)
#target_compile_options(${PROJECT_NAME}_node PUBLIC -li2c)
#target_link_libraries(${PROJECT_NAME}_node -li2c)

target_link_libraries(${PROJECT_NAME}_node controller ${catkin_LIBRARIES})
## Add cmake target dependencies of the library
## as an example, code may need to be generated before libraries
## either from message generation or dynamic reconfigure
# add_dependencies(${PROJECT_NAME} ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Declare a C++ executable
## With catkin_make all packages are built within a single CMake context
## The recommended prefix ensures that target names across packages don't collide
# add_executable(${PROJECT_NAME}_node src/DiRa_PCA8266_Controller_node.cpp)

## Rename C++ executable without prefix
## The above recommended prefix causes long target names, the following renames the
## target back to the shorter version for ease of user use
## e.g. "rosrun someones_pkg node" instead of "rosrun someones_pkg someones_pkg_node"
# set_target_properties(${PROJECT_NAME}_node PROPERTIES OUTPUT_NAME node PREFIX "")

## Add cmake target dependencies of the executable
## same as for the library above
# add_dependencies(${PROJECT_NAME}_node ${${PROJECT_NAME}_EXPORTED_TARGETS} ${catkin_EXPORTED_TARGETS})

## Specify libraries to link a library or executable target against
#target_link_libraries(${PROJECT_NAME}_node
#   ${catkin_LIBRARIES}
#   
# )

#############
## Install ##
#############

# all install targets should use catkin DESTINATION variables
# See http://ros.org/doc/api/catkin/html/adv_user_guide/variables.html

## Mark executable scripts (Python etc.) for installation
## in contrast to setup.py, you can choose the destination
# install(PROGRAMS
#   scripts/my_python_script
#   DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
# )

## Mark executables and/or libraries for installation
install(TARGETS pca9685 controller ${PROJECT_NAME}_node
	ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION} 
	LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
	RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

## Mark cpp header files for installation
install(DIRECTORY include/ /usr/include/i2c/
DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
FILES_MATCHING PATTERN "*.h"
# PATTERN ".svn" EXCLUDE
)

## Mark other files for installation (e.g. launch and bag files, etc.)
# install(FILES
#   # myfile1
#   # myfile2
#   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}
# )

#############
## Testing ##
#############

## Add gtest based cpp test target and link libraries
# catkin_add_gtest(${PROJECT_NAME}-test test/test_DiRa_PCA8266_Controller.cpp)
# if(TARGET ${PROJECT_NAME}-test)
#   target_link_libraries(${PROJECT_NAME}-test ${PROJECT_NAME})
# endif()

## Add folders to be run by python nosetests
# catkin_add_nosetests(test)
