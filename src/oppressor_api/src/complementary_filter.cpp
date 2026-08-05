#include "oppressor_api/complementary_filter.h"

ComplementaryFilterAlgo::ComplementaryFilterAlgo(double alpha) : alpha_(alpha), fused_angle_(0.0){}
double ComplementaryFilterAlgo::update(double imu_angular_velocity, double encoder_angle, double dt) {

  double predicted_angle = fused_angle_ + imu_angular_velocity * dt;
  fused_angle_ = alpha_ * predicted_angle + (1 - alpha_) * encoder_angle;
  return fused_angle_;
}