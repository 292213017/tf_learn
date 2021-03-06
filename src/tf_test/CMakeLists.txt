cmake_minimum_required(VERSION 3.0.2)
project(tf_test)

## Compile as C++11, supported in ROS Kinetic and newer
# add_compile_options(-std=c++11)

## Find catkin macros and libraries
## if COMPONENTS list like find_package(catkin REQUIRED COMPONENTS xyz)
## is used, also find other catkin packages
find_package(catkin REQUIRED COMPONENTS
  nav_msgs
  roscpp
  tf
)

catkin_package(
#  INCLUDE_DIRS include
#  LIBRARIES tf_test
#  CATKIN_DEPENDS nav_msgs roscpp tf
#  DEPENDS system_lib
)

add_subdirectory(./ThirdParty/GeographicLib/)


include_directories(
  include
  ${catkin_INCLUDE_DIRS}
  ${EIGEN3_INCLUDE_DIR}
  /usr/include/eigen3
  ./ThirdParty/GeographicLib/include/
)

add_library(tf_test_lib
  src/odom.cpp
  src/gps_wrapper.cpp
)

target_link_libraries(tf_test_lib
    ${catkin_LIBRARIES}
    libGeographiccc
		)

add_executable(tf_test src/tf_test.cpp src )
target_link_libraries(tf_test  
  tf_test_lib
  )

add_executable(gps_map src/gps_map_node.cpp)
target_link_libraries(gps_map 
  tf_test_lib
  )