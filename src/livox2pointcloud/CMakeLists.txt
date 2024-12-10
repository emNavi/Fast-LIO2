cmake_minimum_required(VERSION 3.0.2)
project(livox2pointcloud)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Release)

find_package(catkin REQUIRED COMPONENTS
  livox_ros_driver2
  roscpp
  sensor_msgs
)

find_library(LIVOX_SDK_LIBRARY VERSION EXACT 2.3.0 liblivox_sdk_static.a /usr/local/lib)

catkin_package(
  CATKIN_DEPENDS livox_ros_driver2 roscpp sensor_msgs
  DEPENDS system_lib
)

add_executable(${PROJECT_NAME}_node src/livox2pointcloud.cpp)
add_dependencies(${PROJECT_NAME}_node ${catkin_EXPORTED_TARGETS})
target_include_directories(${PROJECT_NAME}_node PRIVATE ${catkin_INCLUDE_DIRS})
target_compile_options(${PROJECT_NAME}_node PRIVATE -Wall)

target_link_libraries(${PROJECT_NAME}_node
  ${catkin_LIBRARIES}
)

install(TARGETS ${PROJECT_NAME}_node
 ARCHIVE DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
 LIBRARY DESTINATION ${CATKIN_PACKAGE_LIB_DESTINATION}
 RUNTIME DESTINATION ${CATKIN_GLOBAL_BIN_DESTINATION}
)
