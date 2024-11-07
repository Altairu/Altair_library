#ifndef KINEMATICS_H_
#define KINEMATICS_H_

#include "motor_driver.h"
#include "encoder.h"
#include "pid.h"
#include "math.h"

// 制御モードの定義
typedef enum {
    PWM_MODE,
    PID_MODE
} KinematicsMode;

// ホイールの動作モードを定義
typedef enum {
    OMNI_3,
    OMNI_4,
    MEKANUM
} WheelMode;

// ホイールの配置場所を定義
typedef enum {
    FRONT_RIGHT,
    FRONT_LEFT,
    BACK_RIGHT,
    BACK_LEFT
} WheelPosition;

// デバッグデータ用の構造体
typedef struct {
    double enc[4];
    double out[4];
    double target[4];
} DebugData;

// 足回りの運動学を管理する構造体
typedef struct {
    MotorDriver motors[4];
    Encoder encoders[4];
    Pid pid_controllers[4];
    DebugData debug_data;
    double lx, ly, rx;
    double robot_diameter;
    double wheel_radius;
    KinematicsMode mode;  // 追加: モード設定
} Kinematics;

// 関数プロトタイプ
void Kinematics_InitMotor(Kinematics* kin, WheelPosition pos, MotorDriver* motor,
                          TIM_HandleTypeDef* htimA, uint32_t channelA,
                          TIM_HandleTypeDef* htimB, uint32_t channelB);
void Kinematics_InitEncoder(Kinematics* kin, WheelPosition pos, Encoder* encoder);
void Kinematics_SetPIDGains(Kinematics* kin, WheelPosition pos, double kp, double ki, double kd);
void Kinematics_SetMode(Kinematics* kin, KinematicsMode mode);
void Kinematics_Control(Kinematics* kin, WheelMode mode, double lx, double ly, double rx);
DebugData Kinematics_GetDebugData(Kinematics* kin);

#endif /* KINEMATICS_H_ */
