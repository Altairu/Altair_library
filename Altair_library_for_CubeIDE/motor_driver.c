#include "motor_driver.h"

// 初期化関数
void MotorDriver_Init(MotorDriver* motor, TIM_HandleTypeDef* htimA, uint32_t channelA,
                      TIM_HandleTypeDef* htimB, uint32_t channelB) {
    motor->htimA = htimA;
    motor->channelA = channelA;
    motor->htimB = htimB;
    motor->channelB = channelB;

    // PWM開始
    HAL_TIM_PWM_Start(htimA, channelA);
    HAL_TIM_PWM_Start(htimB, channelB);
}

// 速度設定関数
void MotorDriver_setSpeed(MotorDriver *motor, int speed) {
	int pwm_value;
    if (speed > 100) speed = 99;
    if (speed < -100) speed = -99;

    if (speed > 0) {
        pwm_value = (speed * __HAL_TIM_GET_AUTORELOAD(motor->htimA)) / 100;
        __HAL_TIM_SET_COMPARE(motor->htimA, motor->channelA, pwm_value);
        __HAL_TIM_SET_COMPARE(motor->htimB, motor->channelB, 0);
    } else {
        pwm_value = (-speed * __HAL_TIM_GET_AUTORELOAD(motor->htimA)) / 100;
        __HAL_TIM_SET_COMPARE(motor->htimA, motor->channelA, 0);
        __HAL_TIM_SET_COMPARE(motor->htimB, motor->channelB, pwm_value);
    }
}
