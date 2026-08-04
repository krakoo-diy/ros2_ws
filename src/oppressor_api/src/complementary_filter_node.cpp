#include <chrono>
#include <functional>
#include <memory>

#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "message_filters/subscriber.hpp"
#include "message_filters/synchronizer.hpp"
#include "message_filters/sync_policies/approximate_time.hpp"


typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::msg::Imu, nav_msgs::msg::Odometry> SyncPolicy;




class ComplementaryFilter : public rclcpp::Node {
public:
  ComplementaryFilter() : Node("pub") {
    imu_sub_.subscribe(this, "/imu");
    odom_sub_.subscribe(this, "/diff_drive_controller/odom");
    sync_ = std::make_shared<message_filters::Synchronizer<SyncPolicy>>(SyncPolicy(10), imu_sub_, odom_sub_);
    sync_->registerCallback(std::bind(&ComplementaryFilter::callback, this, std::placeholders::_1, std::placeholders::_2));

  }

private:
    void callback(const sensor_msgs::msg::Imu::ConstSharedPtr &imu, const nav_msgs::msg::Odometry::ConstSharedPtr &odom) {
        RCLCPP_INFO(get_logger(), "Imu angular velocity z: %f, Odom yaw: %f", imu->angular_velocity.z, odom->pose.pose.orientation.z);
    }

    message_filters::Subscriber<sensor_msgs::msg::Imu> imu_sub_;
    message_filters::Subscriber<nav_msgs::msg::Odometry> odom_sub_;
    std::shared_ptr<message_filters::Synchronizer<SyncPolicy>> sync_;

};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto sub_node = std::make_shared<ComplementaryFilter>();
  rclcpp::spin(sub_node);
  rclcpp::shutdown();
  return 0;
}
