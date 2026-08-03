
#include <chrono>
#include <functional>
#include <memory>

#include "geometry_msgs/msg/twist_stamped.hpp"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/float64.hpp"

#include "pid.h"

class controller : public rclcpp::Node {
public:
  controller()
      : Node("controller_node"), pid_(0.005, 0.0, 0.001), last_time_(now()) {
    velo_pub_ = create_publisher<geometry_msgs::msg::TwistStamped>(
        "/diff_drive_controller/cmd_vel", 10);
    error_sub_ = create_subscription<std_msgs::msg::Float64>(
        "/error", 10, [this](std_msgs::msg::Float64::SharedPtr m_error) {
          callback(m_error);
        });
    declare_parameter<float>("Kp", 0.005);
    declare_parameter<float>("Ki", 0.0);
    declare_parameter<float>("Kd", 0.001);
  }

private:
  void callback(std_msgs::msg::Float64::SharedPtr m_error) {
    float kp_;
    float ki_;
    float kd_;
    get_parameter("Kp", kp_);
    get_parameter("ki", ki_);
    get_parameter("kd", kd_);
    PIDController pid(kp_, ki_, kd_);
    // PIDController::PIDController(kp_, ki_, kd_);
    auto error_handler = m_error->data;

    rclcpp::Time current_time = this->now();
    double dt = (current_time - last_time_).seconds();
    last_time_ = current_time; // update for next call

    auto angular_z = -pid_.calculate(error_handler, dt);

    auto val = geometry_msgs::msg::TwistStamped();
    val.twist.linear.x = forward_speed_;
    val.twist.angular.z = angular_z;
    velo_pub_->publish(val);
  }
  rclcpp::Subscription<std_msgs::msg::Float64>::SharedPtr error_sub_;
  rclcpp::Publisher<geometry_msgs::msg::TwistStamped>::SharedPtr velo_pub_;
  PIDController pid_;
  double forward_speed_ = 0.15; // 0.15m/s
  rclcpp::Time last_time_;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto controller_node = std::make_shared<controller>();
  rclcpp::spin(controller_node);
  rclcpp::shutdown();
  return 0;
}