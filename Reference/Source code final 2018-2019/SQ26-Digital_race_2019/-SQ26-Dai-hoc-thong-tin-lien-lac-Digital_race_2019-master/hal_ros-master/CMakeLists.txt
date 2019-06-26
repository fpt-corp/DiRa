cmake_minimum_required(VERSION 2.8.3)
project(hal)

find_package(catkin REQUIRED COMPONENTS
  geometry_msgs
  roscpp
  std_msgs
  message_generation
	actionlib_msgs
)
generate_messages(
  DEPENDENCIES
)
catkin_package(
  INCLUDE_DIRS include
  LIBRARIES ${PROJECT_NAME}
  CATKIN_DEPENDS roscpp std_msg message_runtime
  DEPENDS system_lib
)

include_directories(
 include
  ${catkin_INCLUDE_DIRS}
)

add_library(${PROJECT_NAME}
  src/api_hal.cpp
  src/Hal.cpp
  src/LCDI2C.cpp
)

add_executable(${PROJECT_NAME}_node src/main.cpp)
target_link_libraries(${PROJECT_NAME}_node ${PROJECT_NAME} ${catkin_LIBRARIES})
add_executable(pub src/pub.cpp)
target_link_libraries(pub ${PROJECT_NAME} ${catkin_LIBRARIES})
add_dependencies(hal ${catkin_EXPORTED_TARGETS})

install(TARGETS ${PROJECT_NAME}_node ${PROJECT_NAME}
  LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
  RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)
install(DIRECTORY include/${PROJECT_NAME}/
  DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}
  FILES_MATCHING PATTERN "*.h" PATTERN "*.hpp"
)

