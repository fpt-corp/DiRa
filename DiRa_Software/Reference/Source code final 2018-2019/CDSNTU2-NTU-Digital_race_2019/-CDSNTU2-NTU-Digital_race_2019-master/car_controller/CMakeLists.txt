cmake_minimum_required(VERSION 2.8.3)
project(car_controller)
find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
)
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS roscpp std_msg
  DEPENDS system_lib
)
include_directories(
 include
  ${catkin_INCLUDE_DIRS}
)
add_library(${PROJECT_NAME}
   	src/api_i2c_pwm.cpp
	src/pca9685.cpp
)

add_executable(${PROJECT_NAME}_node src/main.cpp)
target_link_libraries(${PROJECT_NAME}_node ${PROJECT_NAME} ${catkin_LIBRARIES})

install(TARGETS ${PROJECT_NAME}_node ${PROJECT_NAME} 
   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/
   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
   FILES_MATCHING PATTERN "*.h"
)
