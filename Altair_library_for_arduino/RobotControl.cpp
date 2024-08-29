#include "RobotControl.h"

RobotControl::RobotControl(RobotMode mode, double wheel_radius_mm, double turning_radius_mm)
    : mode(mode), wheel_radius_mm(wheel_radius_mm), turning_radius_mm(turning_radius_mm) {
    for (int i = 0; i < 4; i++) {
        motors[i] = nullptr;
        encoders[i] = nullptr;
        pids[i] = nullptr;
        target_speeds[i] = 0.0;
        current_speeds[i] = 0.0;
    }
}

void RobotControl::configureMotor(int motor_index, int pin1, int pin2) {
    if (motor_index >= 0 && motor_index < 4) {
        motors[motor_index] = new MotorDriver(pin1, pin2);
    }
}

void RobotControl::configureEncoder(int motor_index, int pinA, int pinB) {
    if (motor_index >= 0 && motor_index < 4) {
        encoders[motor_index] = new Encoder(pinA, pinB);
    }
}

void RobotControl::setPIDGains(int motor_index, float kp, float ki, float kd, float time_constant) {
    if (motor_index >= 0 && motor_index < 4) {
        pids[motor_index] = new PIDController(kp, ki, kd, time_constant, 0.01);
    }
}

void RobotControl::startControl(double vx_mm_s, double vy_mm_s, double omega_deg_s) {
    double wheel_speeds[4];  // ローカル変数として宣言
    calculateWheelSpeeds(vx_mm_s, vy_mm_s, omega_deg_s, wheel_speeds);
    for (int i = 0; i < 4; i++) {
        setWheelSpeed(i, wheel_speeds[i]);
    }
}

void RobotControl::stopControl() {
    for (int i = 0; i < 4; i++) {
        if (motors[i]) {
            motors[i]->brake();
        }
    }
}

double RobotControl::getMotorOutput(int motor_index) {
    if (motor_index >= 0 && motor_index < 4) {
        // ここでモーターに関連した適切な情報を返します
        return target_speeds[motor_index];  // または motors[motor_index]->getLastSetSpeed();
    }
    return 0.0;
}


double RobotControl::getTargetRPS(int motor_index) {
    if (motor_index >= 0 && motor_index < 4) {
        return target_speeds[motor_index];
    }
    return 0.0;
}

// ここに新しく追加した関数
void RobotControl::setWheelSpeed(int motor_index, double target_rps) {
    if (motor_index >= 0 && motor_index < 4 && motors[motor_index] && pids[motor_index]) {
        double current_rps = encoders[motor_index]->getRPS();
        double pid_output = pids[motor_index]->compute(target_rps, current_rps);
        motors[motor_index]->setSpeed(pid_output);
    }
}
