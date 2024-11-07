#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "stm32f4xx_hal.h"

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

#endif /* MOTOR_DRIVER_H */
