cmake_minimum_required(VERSION 3.8)
project(rosbot_api)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(sensor_msgs REQUIRED)

add_executable(lidar_r src/lidar_r.cpp)
ament_target_dependencies(lidar_r rclcpp sensor_msgs)


install(TARGETS
	lidar_r
	DESTINATION lib/${PROJECT_NAME}/
)



ament_package()
