#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "stm32f4xx_hal.h"

// デフォルトのPWM周波数[Hz]
// ユーザ側で
//   #define MOTOR_DRIVER_DEFAULT_PWM_HZ xxx
// を motor_driver.h をインクルードする前に定義すると上書き可能。
// あるいは、Cファイルのどこかで
//   uint32_t g_motor_driver_default_pwm_hz = xxx;
// と強い定義を置けば、そちらが優先される。
#ifndef MOTOR_DRIVER_DEFAULT_PWM_HZ
#define MOTOR_DRIVER_DEFAULT_PWM_HZ 980U
#endif

typedef struct {
    TIM_HandleTypeDef* htimA;  // タイマーA
    uint32_t channelA;         // タイマーチャンネルA
    TIM_HandleTypeDef* htimB;  // タイマーB
    uint32_t channelB;         // タイマーチャンネルB
} MotorDriver;

// モータードライバを初期化する関数
void MotorDriver_Init(MotorDriver* motor, TIM_HandleTypeDef* htimA, uint32_t channelA,
                      TIM_HandleTypeDef* htimB, uint32_t channelB);

// モーターの速度を設定する関数
void MotorDriver_setSpeed(MotorDriver* motor, int speed);

// PWM 周波数を設定する関数（例: frequency_hz = 980）
HAL_StatusTypeDef MotorDriver_setPwmFrequency(MotorDriver* motor, uint32_t frequency_hz);

#endif /* MOTOR_DRIVER_H */
