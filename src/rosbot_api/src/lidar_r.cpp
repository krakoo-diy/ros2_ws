#include <chrono>
#include <functional>
#include <memory>

#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/laser_scan.hpp"

class LidarInspect : public rclcpp::Node {
public:
  LidarInspect() : Node("lidar_publisher") {
    lidar_subscriber_ = create_subscription<sensor_msgs::msg::LaserScan>(
        "/scan", 10, [this](sensor_msgs::msg::LaserScan::SharedPtr msg) {
          lidar_calback(msg);
        });
  }

private:
  void lidar_calback(const sensor_msgs::msg::LaserScan::SharedPtr msg) {
    for (int i = 0; i < 640; i++) {
      RCLCPP_INFO(get_logger(), "Angle index: %d Distance: %f", i,
                  msg->ranges[i]);
    }
  }
  rclcpp::Subscription<sensor_msgs::msg::LaserScan>::SharedPtr
      lidar_subscriber_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto lidar_sub = std::make_shared<LidarInspect>();
  rclcpp::spin(lidar_sub);
  rclcpp::shutdown();
  return 0;
}