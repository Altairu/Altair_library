// kinematics.h

#ifndef KINEMATICS_H
#define KINEMATICS_H

typedef enum {
    OMNI_3,
    OMNI_4,
    MEKANUM
} WheelMode;

typedef struct {
    float robot_diameter;   // ロボットの直径
    float wheel_radius;     // ホイールの半径
    WheelMode mode;         // 動作モード
} Kinematics;

// 初期化関数
void Kinematics_Init(Kinematics *kinematics, float robot_diameter, float wheel_radius, WheelMode mode);

// モーターの目標速度を計算する関数
void Kinematics_GetTargetSpeeds(Kinematics *kinematics, float lx, float ly, float rx, float *speedFR, float *speedFL, float *speedBR, float *speedBL);

#endif // KINEMATICS_H
