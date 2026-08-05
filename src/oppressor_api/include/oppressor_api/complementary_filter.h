#pragma once

class ComplementaryFilterAlgo {
public:
  ComplementaryFilterAlgo(double alpha);
  double update(double imu_angular_velocity, double encoder_angle, double dt);

private:
  double alpha_;
  double fused_angle_;
};