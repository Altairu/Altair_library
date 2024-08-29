#ifndef ROBOT_CONTROL_H
#define ROBOT_CONTROL_H

#include "MotorDriver.h"
#include "Encoder.h"
#include "PIDController.h"

enum RobotMode {
    Omni4_Mode,
    Omni3_Mode,
    Mecanum_Mode
};

class RobotControl {
public:
    RobotControl(RobotMode mode, double wheel_radius_mm, double turning_radius_mm);

    void configureMotor(int motor_index, int pin1, int pin2);
    void configureEncoder(int motor_index, int pinA, int pinB);
    void setPIDGains(int motor_index, float kp, float ki, float kd, float time_constant);
    void startControl(double vx_mm_s, double vy_mm_s, double omega_deg_s);
    void stopControl();
    double getMotorOutput(int motor_index);
    double getTargetRPS(int motor_index);
    void calculateWheelSpeeds(double vx_mm_s, double vy_mm_s, double omega_deg_s, double* wheel_speeds);

private:
    RobotMode mode;
    double wheel_radius_mm;
    double turning_radius_mm;
    MotorDriver* motors[4];
    Encoder* encoders[4];
    PIDController* pids[4];
    double target_speeds[4];
    double current_speeds[4];

    // 新しく追加する関数の宣言
    void setWheelSpeed(int motor_index, double target_rps);
};

#endif // ROBOT_CONTROL_H
