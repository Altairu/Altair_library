#ifndef TWO_WHEEL_KINEMATICS_H
#define TWO_WHEEL_KINEMATICS_H

#include "mbed.h"

class TwoWheelKinematics {
public:
    // コンストラクタでタイヤ直径とタイヤ間距離を設定する
    TwoWheelKinematics(float wheel_diameter, float wheel_distance)
        : wheel_diameter(wheel_diameter), wheel_distance(wheel_distance) {}

    // ロボットの速度 (v) と旋回角速度 (omega) を入力し、各タイヤの速度を計算する
    void calculateWheelSpeeds(float v, float omega, float &vR, float &vL) {
        float R = v / omega; // 旋回半径の計算

        vR = (R + wheel_distance / 2.0) * omega; // 右車輪の速度
        vL = (R - wheel_distance / 2.0) * omega; // 左車輪の速度
    }

private:
    float wheel_diameter;  // タイヤ直径 (mm)
    float wheel_distance;  // タイヤ間距離 (mm)
};

#endif // TWO_WHEEL_KINEMATICS_H
