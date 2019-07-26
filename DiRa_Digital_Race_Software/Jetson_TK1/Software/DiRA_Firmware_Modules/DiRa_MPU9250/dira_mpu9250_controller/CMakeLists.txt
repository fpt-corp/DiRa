cmake_minimum_required(VERSION 2.8.3)
project(dira_mpu9250_controller)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  sensor_msgs
)

catkin_package(
    LIBRARIES ${PROJECT_NAME}
    CATKIN_DEPENDS roscpp sensor_msgs
)

include_directories( include/
  ${catkin_INCLUDE_DIRS}
)
add_library(${PROJECT_NAME} 
    src/RTFusion.cpp
    src/RTFusionRTQF.cpp
    src/RTMath.cpp
    src/RTFusionKalman4.cpp
    src/RTIMUAccelCal.cpp
    src/RTIMUHal.cpp
    src/RTIMUMagCal.cpp
    src/RTIMUSettings.cpp
    src/RTIMU.cpp
    src/RTIMUGD20M303DLHC.cpp
    src/RTIMUGD20HM303DLHC.cpp
    src/RTIMUGD20HM303D.cpp
    src/RTIMULSM9DS0.cpp
    src/RTIMULSM9DS1.cpp
    src/RTIMUMPU9150.cpp
    src/RTIMUMPU9250.cpp
    src/RTIMUBMX055.cpp
    src/RTIMUBNO055.cpp
    src/RTIMUNull.cpp
    src/RTPressure.cpp
    src/RTPressureBMP180.cpp
    src/RTPressureLPS25H.cpp
    src/RTPressureMS5611.cpp
    src/RTPressureMS5637.cpp
    src/RTHumidity.cpp
    src/RTHumidityHTS221.cpp
    src/RTHumidityHTU21D.cpp
)
add_executable(${PROJECT_NAME}_node src/dira_mpu9250_controller.cpp)

target_link_libraries(${PROJECT_NAME}_node ${PROJECT_NAME}
  ${catkin_LIBRARIES}
  #RTIMULib
)
install(TARGETS ${PROJECT_NAME}_node
   ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
   RUNTIME DESTINATION ${CATKIN_PACKAGE_BIN_DESTINATION}
)

install(DIRECTORY include/
   DESTINATION ${CATKIN_PACKAGE_INCLUDE_DESTINATION}/include
   FILES_MATCHING PATTERN "*.h"
)
install(DIRECTORY config/
   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/config
   FILES_MATCHING PATTERN "*.ini"
)
install(DIRECTORY launch/
   DESTINATION ${CATKIN_PACKAGE_SHARE_DESTINATION}/launch
   FILES_MATCHING PATTERN "*.launch"
)

