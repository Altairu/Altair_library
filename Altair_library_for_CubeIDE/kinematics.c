#include "kinematics.h"

// モーターを初期化する関数（タイマーとチャンネルを個別設定）
void Kinematics_InitMotor(Kinematics* kin, WheelPosition pos, MotorDriver* motor,
                          TIM_HandleTypeDef* htimA, uint32_t channelA,
                          TIM_HandleTypeDef* htimB, uint32_t channelB) {
    MotorDriver_Init(motor, htimA, channelA, htimB, channelB);
    kin->motors[pos] = *motor;
}

// エンコーダを初期化する関数
void Kinematics_InitEncoder(Kinematics* kin, WheelPosition pos, Encoder* encoder) {
    kin->encoders[pos] = *encoder;
}

// PIDゲインを設定する関数
void Kinematics_SetPIDGains(Kinematics* kin, WheelPosition pos, double kp, double ki, double kd) {
    Pid_setGain(&kin->pid_controllers[pos], kp, ki, kd, 0.0); // time_constantに0を指定
}

// モードを設定する関数
void Kinematics_SetMode(Kinematics* kin, KinematicsMode mode) {
    kin->mode = mode;
}

// 指定されたホイールモードで運動を制御する関数
void Kinematics_Control(Kinematics* kin, WheelMode mode, double lx, double ly, double rx) {
    kin->lx = lx;
    kin->ly = ly;
    kin->rx = rx * M_PI / 180.0;  // 角速度をラジアンに変換
    double DI = kin->robot_diameter;
    double WH = kin->wheel_radius;

    // モード別にホイールの目標速度を計算
    switch (mode) {
        case OMNI_3:
            kin->debug_data.target[0] = (-lx + DI * rx) / (2 * M_PI * WH);
            kin->debug_data.target[1] = (lx / 2 - ly * sqrt(3) / 2 + DI * rx) / (2 * M_PI * WH);
            kin->debug_data.target[2] = (lx / 2 + ly * sqrt(3) / 2 + DI * rx) / (2 * M_PI * WH);
            break;

        case OMNI_4:
            kin->debug_data.target[0] = (-(sqrt(2) / 2) * lx + (sqrt(2) / 2) * ly + DI * rx) / (2 * M_PI * WH);
            kin->debug_data.target[1] = ((sqrt(2) / 2) * lx + (sqrt(2) / 2) * ly + DI * rx) / (2 * M_PI * WH);
            kin->debug_data.target[2] = ((sqrt(2) / 2) * lx - (sqrt(2) / 2) * ly + DI * rx) / (2 * M_PI * WH);
            kin->debug_data.target[3] = (-(sqrt(2) / 2) * lx - (sqrt(2) / 2) * ly + DI * rx) / (2 * M_PI * WH);
            break;

        case MEKANUM:
            kin->debug_data.target[0] = ((lx - ly) / sqrt(2) + DI * rx) / (2 * M_PI * WH);
            kin->debug_data.target[1] = ((-lx - ly) / sqrt(2) + DI * rx) / (2 * M_PI * WH);
            kin->debug_data.target[2] = ((-lx + ly) / sqrt(2) + DI * rx) / (2 * M_PI * WH);
            kin->debug_data.target[3] = ((lx + ly) / sqrt(2) + DI * rx) / (2 * M_PI * WH);
            break;
    }

    // 各ホイールの速度を設定
    for (int i = 0; i < 4; i++) {
        if (kin->mode == PID_MODE) {
            double enc_value = Encoder_Read(&kin->encoders[i]);
            kin->debug_data.enc[i] = enc_value;
            kin->debug_data.out[i] = Pid_control(&kin->pid_controllers[i], kin->debug_data.target[i], enc_value, 10);
            MotorDriver_setSpeed(&kin->motors[i], kin->debug_data.out[i]);
        } else if (kin->mode == PWM_MODE) {
            kin->debug_data.out[i] = kin->debug_data.target[i];
            MotorDriver_setSpeed(&kin->motors[i], kin->debug_data.out[i]);
        }
    }
}

// デバッグデータを取得する関数
DebugData Kinematics_GetDebugData(Kinematics* kin) {
    return kin->debug_data;
}
