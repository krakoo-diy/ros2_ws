#include <chrono>
#include <functional>
#include <memory>

#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"

class ComplementaryFilter : public rclcpp::Node {
public:
  ComplementaryFilter() : Node("pub") {
    imu_subscriber_ = create_subscription<sensor_msgs::msg::Imu>(
        "/imu", 10, [this](sensor_msgs::msg::Imu::SharedPtr imu_data) {
          imu_callback(imu_data);
        });
    odom_subscriber_ = create_subscription<nav_msgs::msg::Odometry::SharedPtr>("/diff_drive_controller", 10,[this](nav_msgs::msg::Odometry::SharedPtr odom_data) {odom_callback(odom_data);});
  }

private:
  void imu_callback(const sensor_msgs::msg::Imu::SharedPtr imu_data) const {
    RCLCPP_INFO(get_logger(), "Angular velocity: %f",
                imu_data->angular_velocity.z);
  }
  void odom_callback(nav_msgs::msg::Odometry::SharedPtr odom_data) {
    RCLCPP_INFO(get_logger(), "Odom %f", odom_data->pose.pose.position.x);
  }
  rclcpp::Subscription<sensor_msgs::msg::Imu>::SharedPtr imu_subscriber_;
  rclcpp::Subscription<nav_msgs::msg::Odometry>::SharedPtr odom_subscriber_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto sub_node = std::make_shared<ComplementaryFilter>();
  rclcpp::spin(sub_node);
  rclcpp::shutdown();
  return 0;
}
