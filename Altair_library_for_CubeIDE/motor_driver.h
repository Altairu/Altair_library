#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_tim.h" // TIM_HandleTypeDefのためのインクルード

// MotorDriver構造体の定義
typedef struct {
    TIM_HandleTypeDef *htim;
    uint32_t channel1;
    uint32_t channel2;
} MotorDriver;

// MotorDriverの初期化
void MotorDriver_Init(MotorDriver *motor, TIM_HandleTypeDef *htim, uint32_t channel1, uint32_t channel2);

// モーターの速度設定関数
void MotorDriver_setSpeed(MotorDriver *motor, int speed);

#endif // MOTOR_DRIVER_H
