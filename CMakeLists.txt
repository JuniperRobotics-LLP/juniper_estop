cmake_minimum_required(VERSION 3.5)
project(juniper_estop)

# Default to C++14
if(NOT CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 14)
endif()

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(juniper_board_msgs REQUIRED)

add_executable(juniper_estop src/juniper_estop.cpp)
ament_target_dependencies(juniper_estop rclcpp std_msgs geometry_msgs juniper_board_msgs)

install(TARGETS
  juniper_estop
  DESTINATION lib/${PROJECT_NAME})

ament_package()