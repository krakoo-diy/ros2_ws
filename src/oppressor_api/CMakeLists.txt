cmake_minimum_required(VERSION 3.8)
project(oppressor_api)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(geometry_msgs REQUIRED)
find_package(std_msgs REQUIRED)
find_package(sensor_msgs REQUIRED)
find_package(nav_msgs REQUIRED)
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(OpenCV REQUIRED)
find_package(message_filters REQUIRED)
find_package(tf2)
find_package(tf2_geometry_msgs)



add_executable(image_processing src/image_processing.cpp)
ament_target_dependencies(image_processing rclcpp std_msgs geometry_msgs cv_bridge image_transport OpenCV sensor_msgs)

add_executable(controller src/controller.cpp src/pid.cpp)
ament_target_dependencies(controller rclcpp std_msgs geometry_msgs)

target_include_directories(controller PUBLIC include/)

add_executable(complementary_filter 
	src/complementary_filter_node.cpp
	src/complementary_filter.cpp
)
ament_target_dependencies(complementary_filter rclcpp sensor_msgs nav_msgs message_filters tf2 tf2_geometry_msgs)

target_include_directories(complementary_filter PUBLIC include/)


install(TARGETS 
	image_processing
	controller
	complementary_filter
	DESTINATION lib/${PROJECT_NAME}/
)




ament_package()
