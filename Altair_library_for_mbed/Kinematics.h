#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <cmath>

// モータ制御データ構造体の定義
struct MotorData {
    double target_value;  // 制御データ（RPSまたはmm/s）
    double pwm_command;
};

struct MotorControlData {
    MotorData motor_data[4];
};

enum ControlMode {
    RPS_MODE,
    MMPS_MODE // mm/s
};

// 基底クラス Kinematics の定義
class Kinematics {
public:
    virtual void calc(double vx_mm_s, double vy_mm_s, double omega_deg_s, MotorControlData& motor_control_data) = 0;
    virtual ~Kinematics() = default; // 仮想デストラクタ
};

// Mecanum クラス
class Mecanum : public Kinematics {
public:
    Mecanum(double wheel_radius_mm, double turning_radius_mm, ControlMode mode)
        : wheel_radius_mm(wheel_radius_mm), turning_radius_mm(turning_radius_mm), mode(mode) {}

    void calc(double vx_mm_s, double vy_mm_s, double omega_deg_s, MotorControlData& motor_control_data) override {
        double omega_rad_s = omega_deg_s * M_PI / 180.0; // 角速度をラジアン毎秒に変換
        motor_control_data.motor_data[0].target_value = computeTargetValue((-vx_mm_s + vy_mm_s) / std::sqrt(2) + turning_radius_mm * omega_rad_s);
        motor_control_data.motor_data[1].target_value = computeTargetValue((-vx_mm_s - vy_mm_s) / std::sqrt(2) + turning_radius_mm * omega_rad_s);
        motor_control_data.motor_data[2].target_value = computeTargetValue((vx_mm_s - vy_mm_s) / std::sqrt(2) + turning_radius_mm * omega_rad_s);
        motor_control_data.motor_data[3].target_value = computeTargetValue((vx_mm_s + vy_mm_s) / std::sqrt(2) + turning_radius_mm * omega_rad_s);
    }

private:
    double wheel_radius_mm;   // 車輪の半径 (mm)
    double turning_radius_mm; // 旋回半径 (mm)
    ControlMode mode;

    double computeTargetValue(double input_mm_s) {
        if (mode == RPS_MODE) {
            return input_mm_s / (2 * M_PI * wheel_radius_mm); // RPSに変換
        } else { // MMPS_MODE
            return input_mm_s; // mm/sのまま返す
        }
    }
};

// Omni3 クラス
class Omni3 : public Kinematics {
public:
    Omni3(double wheel_radius_mm, double turning_radius_mm, ControlMode mode)
        : wheel_radius_mm(wheel_radius_mm), turning_radius_mm(turning_radius_mm), mode(mode) {}

    void calc(double vx_mm_s, double vy_mm_s, double omega_deg_s, MotorControlData& motor_control_data) override {
        double omega_rad_s = omega_deg_s * M_PI / 180.0; // 角速度をラジアン毎秒に変換
        motor_control_data.motor_data[0].target_value = computeTargetValue(-vx_mm_s + turning_radius_mm * omega_rad_s);
        motor_control_data.motor_data[1].target_value = computeTargetValue(vx_mm_s / 2 - vy_mm_s * std::sqrt(3) / 2 + turning_radius_mm * omega_rad_s);
        motor_control_data.motor_data[2].target_value = computeTargetValue(vx_mm_s / 2 + vy_mm_s * std::sqrt(3) / 2 + turning_radius_mm * omega_rad_s);
    }

private:
    double wheel_radius_mm;   // 車輪の半径 (mm)
    double turning_radius_mm; // 旋回半径 (mm)
    ControlMode mode;

    double computeTargetValue(double input_mm_s) {
        if (mode == RPS_MODE) {
            return input_mm_s / (2 * M_PI * wheel_radius_mm); // RPSに変換
        } else { // MMPS_MODE
            return input_mm_s; // mm/sのまま返す
        }
    }
};
// Omni4 クラス
class Omni4 : public Kinematics {
public:
    Omni4(double wheel_radius_mm, double turning_radius_mm, ControlMode mode)
        : wheel_radius_mm(wheel_radius_mm), turning_radius_mm(turning_radius_mm), mode(mode) {}

    void calc(double vx_mm_s, double vy_mm_s, double omega_deg_s, MotorControlData& motor_control_data) override {
        double omega_rad_s = omega_deg_s * M_PI / 180.0; // 角速度をラジアン毎秒に変換
        motor_control_data.motor_data[0].target_value = computeTargetValue(vx_mm_s - vy_mm_s - turning_radius_mm * omega_rad_s); // 左前
        motor_control_data.motor_data[1].target_value = computeTargetValue(vx_mm_s + vy_mm_s + turning_radius_mm * omega_rad_s); // 右前
        motor_control_data.motor_data[2].target_value = computeTargetValue(vx_mm_s - vy_mm_s + turning_radius_mm * omega_rad_s); // 右後
        motor_control_data.motor_data[3].target_value = computeTargetValue(vx_mm_s + vy_mm_s - turning_radius_mm * omega_rad_s); // 左後
    }

private:
    double wheel_radius_mm;   // 車輪の半径 (mm)
    double turning_radius_mm; // 旋回半径 (mm)
    ControlMode mode;

    double computeTargetValue(double input_mm_s) {
        if (mode == RPS_MODE) {
            return input_mm_s / (2 * M_PI * wheel_radius_mm); // RPSに変換
        } else { // MMPS_MODE
            return input_mm_s; // mm/sのまま返す
        }
    }
};

#endif // KINEMATICS_H
