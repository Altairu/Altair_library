#ifndef ROBOT_CONTROL_H
#define ROBOT_CONTROL_H

#include "mbed.h"
#include "MotorDriver.h"
#include "encoder.h"
#include "PIDController.h"
#include "Kinematics.h"

enum RobotMode {
    Mecanum_Mode,
    Omni3_Mode,
    Omni4_Mode
};

class RobotControl {
public:
    RobotControl(RobotMode mode, double wheel_radius_mm, double turning_radius_mm, ControlMode control_mode);

    void configureMotor(int motor_index, PinName pin1, PinName pin2);
    void configureEncoder(int motor_index, PinName pinA, PinName pinB);
    void setExternalRPS(int motor_index, double rps);
    void setPIDGains(int motor_index, float kp, float ki, float kd, float time_constant);
    void startControl(double vx_mm_s, double vy_mm_s, double omega_deg_s);
    void stopControl();
    double getMotorOutput(int motor_index);
    
    // 追加: 目標RPSを取得するメソッド
    double getTargetRPS(int motor_index);

private:
    RobotMode mode;
    ControlMode control_mode;
    MotorDriver* motors[4];
    Encoder* encoders[4] = {nullptr};
    PIDController* pids[4];
    MotorControlData motor_control_data;
    Kinematics* kinematics;
    Thread motor_control_thread;
    bool running;
    bool thread_started;

    double external_rps[4];
    bool use_external_rps[4];

    void controlLoop();
};

#endif // ROBOT_CONTROL_H
