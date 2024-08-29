#include "robot_control.h"

RobotControl::RobotControl(RobotMode mode, double wheel_radius_mm, double turning_radius_mm, ControlMode control_mode)
    : mode(mode), control_mode(control_mode), running(true), thread_started(false) {
    switch (mode) {
        case Mecanum_Mode:
            kinematics = new Mecanum(wheel_radius_mm, turning_radius_mm, control_mode);
            break;
        case Omni3_Mode:
            kinematics = new Omni3(wheel_radius_mm, turning_radius_mm, control_mode);
            break;
        case Omni4_Mode:
            kinematics = new Omni4(wheel_radius_mm, turning_radius_mm, control_mode);
            break;
    }

    for (int i = 0; i < 4; i++) {
        external_rps[i] = 0.0;
        use_external_rps[i] = false;
    }
}

void RobotControl::configureMotor(int motor_index, PinName pin1, PinName pin2) {
    if (motor_index >= 0 && motor_index < 4) {
        motors[motor_index] = new MotorDriver(pin1, pin2);
    }
}

void RobotControl::configureEncoder(int motor_index, PinName pinA, PinName pinB) {
    if (motor_index >= 0 && motor_index < 4) {
        encoders[motor_index] = new Encoder(pinA, pinB);
        use_external_rps[motor_index] = false;
    }
}

void RobotControl::setExternalRPS(int motor_index, double rps) {
    if (motor_index >= 0 && motor_index < 4) {
        external_rps[motor_index] = rps;
        use_external_rps[motor_index] = true;
    }
}

void RobotControl::setPIDGains(int motor_index, float kp, float ki, float kd, float time_constant) {
    if (motor_index >= 0 && motor_index < 4) {
        pids[motor_index] = new PIDController(kp, ki, kd, time_constant, 0.01);
    }
}

void RobotControl::startControl(double vx_mm_s, double vy_mm_s, double omega_deg_s) {
    if (!thread_started) {
        motor_control_thread.start(callback(this, &RobotControl::controlLoop));
        thread_started = true;
    }
    kinematics->calc(vx_mm_s, vy_mm_s, omega_deg_s, motor_control_data);
}

void RobotControl::stopControl() {
    running = false;
    if (thread_started) {
        motor_control_thread.terminate();
        thread_started = false;
    }
}

void RobotControl::controlLoop() {
    while (running) {
        for (int i = 0; i < 4; i++) {
            double current_rps;
            if (encoders[i] != nullptr && !use_external_rps[i]) {
                current_rps = encoders[i]->getRPS();
            } else {
                current_rps = external_rps[i];
            }
            double control_signal = pids[i]->compute(motor_control_data.motor_data[i].target_value, current_rps);
            motors[i]->setSpeed(control_signal * 100);
        }
        ThisThread::sleep_for(10ms);
    }
}

double RobotControl::getMotorOutput(int motor_index) {
    if (motor_index >= 0 && motor_index < 4) {
        return motor_control_data.motor_data[motor_index].pwm_command;
    }
    return 0.0;
}

// 追加: 目標RPSを取得するメソッド
double RobotControl::getTargetRPS(int motor_index) {
    if (motor_index >= 0 && motor_index < 4) {
        return motor_control_data.motor_data[motor_index].target_value;
    }
    return 0.0;
}
