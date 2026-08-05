#include <chrono>
#include <functional>
#include <memory>

#include "nav_msgs/msg/odometry.hpp"
#include "rclcpp/rclcpp.hpp"
#include "sensor_msgs/msg/imu.hpp"
#include "message_filters/subscriber.hpp"
#include "message_filters/synchronizer.hpp"
#include "message_filters/sync_policies/approximate_time.hpp"
#include "tf2_geometry_msgs/tf2_geometry_msgs.hpp"
#include "tf2/utils.hpp"

#include "oppressor_api/complementary_filter.h"


typedef message_filters::sync_policies::ApproximateTime<sensor_msgs::msg::Imu, nav_msgs::msg::Odometry> SyncPolicy;




class ComplementaryFilter : public rclcpp::Node {
public:
  ComplementaryFilter() : Node("complementary_filter"),  cp_(0.98), last_time_(now()) {
    imu_sub_.subscribe(this, "/imu");
    odom_sub_.subscribe(this, "/diff_drive_controller/odom");
    sync_ = std::make_shared<message_filters::Synchronizer<SyncPolicy>>(SyncPolicy(10), imu_sub_, odom_sub_);
    sync_->registerCallback(std::bind(&ComplementaryFilter::callback, this, std::placeholders::_1, std::placeholders::_2));

  }

private:
    void callback(const sensor_msgs::msg::Imu::ConstSharedPtr &imu, const nav_msgs::msg::Odometry::ConstSharedPtr &odom) {
        double roll, pitch, yow;
        tf2::Quaternion q(odom->pose.pose.orientation.x, odom->pose.pose.orientation.y, odom->pose.pose.orientation.z, odom->pose.pose.orientation.w);
        tf2::Matrix3x3(q).getRPY(roll, pitch, yow);

        double imu_angularV_z = imu->angular_velocity.z;

        rclcpp::Time current_time = now();
        double dt = (current_time - last_time_).seconds();
        last_time_ = current_time;
        
        auto f_angle = cp_.update(imu_angularV_z, yow, dt);
        RCLCPP_INFO(get_logger(), "Fusioned angle: %f ", f_angle);

        //RCLCPP_INFO(get_logger(), "Imu angular velocity z: %f, Odom yaw: %f", imu->angular_velocity.z, odom->pose.pose.orientation.z);

    }

    message_filters::Subscriber<sensor_msgs::msg::Imu> imu_sub_;
    message_filters::Subscriber<nav_msgs::msg::Odometry> odom_sub_;
    std::shared_ptr<message_filters::Synchronizer<SyncPolicy>> sync_;
    ComplementaryFilterAlgo cp_;
    rclcpp::Time last_time_;

};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto sub_node = std::make_shared<ComplementaryFilter>();
  rclcpp::spin(sub_node);
  rclcpp::shutdown();
  return 0;
}
