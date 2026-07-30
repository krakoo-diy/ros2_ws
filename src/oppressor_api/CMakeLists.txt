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
find_package(cv_bridge REQUIRED)
find_package(image_transport REQUIRED)
find_package(OpenCV REQUIRED)


add_executable(image_processing src/image_processing.cpp)
ament_target_dependencies(image_processing rclcpp std_msgs geometry_msgs cv_bridge image_transport OpenCV sensor_msgs)



install(TARGETS 
	image_processing
	DESTINATION lib/${PROJECT_NAME}/
)




ament_package()
