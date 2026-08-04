#pragma once
class PIDController {
public:
  PIDController(double kp, double ki, double kd);
  double calculate(double error, double dt);
  void reset();
  void setGains(double new_Kp, double new_Ki, double new_Kd);

private:
  double kp_, ki_, kd_;
  double integral_;
  double previous_error_;
};