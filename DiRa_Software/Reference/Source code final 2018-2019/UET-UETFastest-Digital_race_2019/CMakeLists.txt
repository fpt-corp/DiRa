cmake_minimum_required(VERSION 2.8)

project(cds2019)

find_package( OpenCV REQUIRED )
find_package( Threads REQUIRED )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

if(${CMAKE_SYSTEM_PROCESSOR} MATCHES "x86_64") 
	message(${CMAKE_SYSTEM_PROCESSOR})
	include_directories(
		${CMAKE_SOURCE_DIR}/include
		${CMAKE_SOURCE_DIR}/include/x86_64
	)
	FILE(GLOB ADDITIONAL_EXECUTABLE src/*.cpp src/x86_64/*.cpp)
	set(ADDITIONAL_LIBRARIES "")
else()
	message(${CMAKE_SYSTEM_PROCESSOR})
	include_directories(
		${CMAKE_SOURCE_DIR}/include
		${CMAKE_SOURCE_DIR}/include/aarch64
		${CMAKE_SOURCE_DIR}/include/aarch64/OpenNI-Linux-Arm64-2.3.0.55/Include
	)
	link_directories (${CMAKE_SOURCE_DIR}/include/aarch64/OpenNI-Linux-Arm64-2.3.0.55/Redist)
	FILE(GLOB ADDITIONAL_EXECUTABLE src/*.cpp src/aarch64/*.cpp)
	set(ADDITIONAL_LIBRARIES OpenNI2 RTIMULib)
endif()

add_executable(${PROJECT_NAME} 
	src/main.cpp
	${ADDITIONAL_EXECUTABLE}
)
target_link_libraries(${PROJECT_NAME} 
	${OpenCV_LIBS} 
	${CMAKE_THREAD_LIBS_INIT}
	${ADDITIONAL_LIBRARIES}
)
