#include "pid.h"

PIDController::PIDController(double kp, double ki, double kd)
    : kp_(kp), ki_(ki), kd_(kd), integral_(0.0), previous_error_(0.0) {}
double PIDController::calculate(double error, double dt) {
  double proportional = kp_ * error;
  integral_ += error * dt;
  double integral_term = ki_ * integral_;
  double derivative = (error - previous_error_) / dt;
  double derivative_term = kd_ * derivative;
  double output = proportional + integral_term + derivative_term;
  previous_error_ = error;
  return output;
}

void PIDController::reset() {
  integral_ = 0.0;
  previous_error_ = 0.0;
}