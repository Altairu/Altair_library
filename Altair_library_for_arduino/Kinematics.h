#ifndef KINEMATICS_H
#define KINEMATICS_H

#include <cmath>

enum KinematicsMode {
    Mecanum,
    Omni3,
    Omni4
};

class Kinematics {
public:
    Kinematics(KinematicsMode mode, double R, double rw)
        : mode(mode), R(R), rw(rw) {}

    void calculate(double vx, double vy, double omega, double* wheel_speeds) {
        switch (mode) {
            case Mecanum:
                wheel_speeds[0] = (1.0 / rw) * (vx - vy - R * omega);  // 左前
                wheel_speeds[1] = (1.0 / rw) * (vx + vy + R * omega);  // 右前
                wheel_speeds[2] = (1.0 / rw) * (vx - vy + R * omega);  // 右後
                wheel_speeds[3] = (1.0 / rw) * (vx + vy - R * omega);  // 左後
                break;

            case Omni3:
                wheel_speeds[0] = (1.0 / rw) * (vx + R * omega);  // 前
                wheel_speeds[1] = (1.0 / rw) * (-vx / 2.0 + (std::sqrt(3) / 2.0) * vy + R * omega);  // 左後
                wheel_speeds[2] = (1.0 / rw) * (-vx / 2.0 - (std::sqrt(3) / 2.0) * vy + R * omega);  // 右後
                break;

            case Omni4:
                wheel_speeds[0] = (1.0 / rw) * (-std::sqrt(2)/2.0 * vx + std::sqrt(2)/2.0 * vy + R * omega);  // 左前
                wheel_speeds[1] = (1.0 / rw) * (std::sqrt(2)/2.0 * vx + std::sqrt(2)/2.0 * vy + R * omega);   // 右前
                wheel_speeds[2] = (1.0 / rw) * (std::sqrt(2)/2.0 * vx - std::sqrt(2)/2.0 * vy + R * omega);   // 右後
                wheel_speeds[3] = (1.0 / rw) * (-std::sqrt(2)/2.0 * vx - std::sqrt(2)/2.0 * vy + R * omega);  // 左後
                break;
        }
    }

private:
    KinematicsMode mode;
    double R;  // ロボットの旋回半径
    double rw; // オムニホイールの半径
};

#endif // KINEMATICS_H
