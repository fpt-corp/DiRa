# generated from genmsg/cmake/pkg-genmsg.cmake.em

message(STATUS "the_walkers: 1 messages, 0 services")

set(MSG_I_FLAGS "-Ithe_walkers:/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg;-Isensor_msgs:/opt/ros/lunar/share/sensor_msgs/cmake/../msg;-Igeometry_msgs:/opt/ros/lunar/share/geometry_msgs/cmake/../msg;-Istd_msgs:/opt/ros/lunar/share/std_msgs/cmake/../msg")

# Find all generators
find_package(gencpp REQUIRED)
find_package(geneus REQUIRED)
find_package(genlisp REQUIRED)
find_package(gennodejs REQUIRED)
find_package(genpy REQUIRED)

add_custom_target(the_walkers_generate_messages ALL)

# verify that message/service dependencies have not changed since configure



get_filename_component(_filename "/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg/Line.msg" NAME_WE)
add_custom_target(_the_walkers_generate_messages_check_deps_${_filename}
  COMMAND ${CATKIN_ENV} ${PYTHON_EXECUTABLE} ${GENMSG_CHECK_DEPS_SCRIPT} "the_walkers" "/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg/Line.msg" ""
)

#
#  langs = gencpp;geneus;genlisp;gennodejs;genpy
#

### Section generating for lang: gencpp
### Generating Messages
_generate_msg_cpp(the_walkers
  "/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg/Line.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/the_walkers
)

### Generating Services

### Generating Module File
_generate_module_cpp(the_walkers
  ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/the_walkers
  "${ALL_GEN_OUTPUT_FILES_cpp}"
)

add_custom_target(the_walkers_generate_messages_cpp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_cpp}
)
add_dependencies(the_walkers_generate_messages the_walkers_generate_messages_cpp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg/Line.msg" NAME_WE)
add_dependencies(the_walkers_generate_messages_cpp _the_walkers_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(the_walkers_gencpp)
add_dependencies(the_walkers_gencpp the_walkers_generate_messages_cpp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS the_walkers_generate_messages_cpp)

### Section generating for lang: geneus
### Generating Messages
_generate_msg_eus(the_walkers
  "/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg/Line.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/the_walkers
)

### Generating Services

### Generating Module File
_generate_module_eus(the_walkers
  ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/the_walkers
  "${ALL_GEN_OUTPUT_FILES_eus}"
)

add_custom_target(the_walkers_generate_messages_eus
  DEPENDS ${ALL_GEN_OUTPUT_FILES_eus}
)
add_dependencies(the_walkers_generate_messages the_walkers_generate_messages_eus)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg/Line.msg" NAME_WE)
add_dependencies(the_walkers_generate_messages_eus _the_walkers_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(the_walkers_geneus)
add_dependencies(the_walkers_geneus the_walkers_generate_messages_eus)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS the_walkers_generate_messages_eus)

### Section generating for lang: genlisp
### Generating Messages
_generate_msg_lisp(the_walkers
  "/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg/Line.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/the_walkers
)

### Generating Services

### Generating Module File
_generate_module_lisp(the_walkers
  ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/the_walkers
  "${ALL_GEN_OUTPUT_FILES_lisp}"
)

add_custom_target(the_walkers_generate_messages_lisp
  DEPENDS ${ALL_GEN_OUTPUT_FILES_lisp}
)
add_dependencies(the_walkers_generate_messages the_walkers_generate_messages_lisp)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg/Line.msg" NAME_WE)
add_dependencies(the_walkers_generate_messages_lisp _the_walkers_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(the_walkers_genlisp)
add_dependencies(the_walkers_genlisp the_walkers_generate_messages_lisp)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS the_walkers_generate_messages_lisp)

### Section generating for lang: gennodejs
### Generating Messages
_generate_msg_nodejs(the_walkers
  "/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg/Line.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/the_walkers
)

### Generating Services

### Generating Module File
_generate_module_nodejs(the_walkers
  ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/the_walkers
  "${ALL_GEN_OUTPUT_FILES_nodejs}"
)

add_custom_target(the_walkers_generate_messages_nodejs
  DEPENDS ${ALL_GEN_OUTPUT_FILES_nodejs}
)
add_dependencies(the_walkers_generate_messages the_walkers_generate_messages_nodejs)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg/Line.msg" NAME_WE)
add_dependencies(the_walkers_generate_messages_nodejs _the_walkers_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(the_walkers_gennodejs)
add_dependencies(the_walkers_gennodejs the_walkers_generate_messages_nodejs)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS the_walkers_generate_messages_nodejs)

### Section generating for lang: genpy
### Generating Messages
_generate_msg_py(the_walkers
  "/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg/Line.msg"
  "${MSG_I_FLAGS}"
  ""
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/the_walkers
)

### Generating Services

### Generating Module File
_generate_module_py(the_walkers
  ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/the_walkers
  "${ALL_GEN_OUTPUT_FILES_py}"
)

add_custom_target(the_walkers_generate_messages_py
  DEPENDS ${ALL_GEN_OUTPUT_FILES_py}
)
add_dependencies(the_walkers_generate_messages the_walkers_generate_messages_py)

# add dependencies to all check dependencies targets
get_filename_component(_filename "/home/nvidia/catkin_ws/src/lhucds/detect/the_walkers/msg/Line.msg" NAME_WE)
add_dependencies(the_walkers_generate_messages_py _the_walkers_generate_messages_check_deps_${_filename})

# target for backward compatibility
add_custom_target(the_walkers_genpy)
add_dependencies(the_walkers_genpy the_walkers_generate_messages_py)

# register target for catkin_package(EXPORTED_TARGETS)
list(APPEND ${PROJECT_NAME}_EXPORTED_TARGETS the_walkers_generate_messages_py)



if(gencpp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/the_walkers)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gencpp_INSTALL_DIR}/the_walkers
    DESTINATION ${gencpp_INSTALL_DIR}
  )
endif()
if(TARGET sensor_msgs_generate_messages_cpp)
  add_dependencies(the_walkers_generate_messages_cpp sensor_msgs_generate_messages_cpp)
endif()

if(geneus_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/the_walkers)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${geneus_INSTALL_DIR}/the_walkers
    DESTINATION ${geneus_INSTALL_DIR}
  )
endif()
if(TARGET sensor_msgs_generate_messages_eus)
  add_dependencies(the_walkers_generate_messages_eus sensor_msgs_generate_messages_eus)
endif()

if(genlisp_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/the_walkers)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genlisp_INSTALL_DIR}/the_walkers
    DESTINATION ${genlisp_INSTALL_DIR}
  )
endif()
if(TARGET sensor_msgs_generate_messages_lisp)
  add_dependencies(the_walkers_generate_messages_lisp sensor_msgs_generate_messages_lisp)
endif()

if(gennodejs_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/the_walkers)
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${gennodejs_INSTALL_DIR}/the_walkers
    DESTINATION ${gennodejs_INSTALL_DIR}
  )
endif()
if(TARGET sensor_msgs_generate_messages_nodejs)
  add_dependencies(the_walkers_generate_messages_nodejs sensor_msgs_generate_messages_nodejs)
endif()

if(genpy_INSTALL_DIR AND EXISTS ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/the_walkers)
  install(CODE "execute_process(COMMAND \"/usr/bin/python\" -m compileall \"${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/the_walkers\")")
  # install generated code
  install(
    DIRECTORY ${CATKIN_DEVEL_PREFIX}/${genpy_INSTALL_DIR}/the_walkers
    DESTINATION ${genpy_INSTALL_DIR}
  )
endif()
if(TARGET sensor_msgs_generate_messages_py)
  add_dependencies(the_walkers_generate_messages_py sensor_msgs_generate_messages_py)
endif()
