#include "motor_driver.h"

// 初期化関数
void MotorDriver_Init(MotorDriver *motor, TIM_HandleTypeDef *htim, uint32_t channel1, uint32_t channel2) {
    motor->htim = htim;
    motor->channel1 = channel1;
    motor->channel2 = channel2;

    HAL_TIM_PWM_Start(htim, channel1);
    HAL_TIM_PWM_Start(htim, channel2);
}

// 速度設定関数
void MotorDriver_setSpeed(MotorDriver *motor, int speed) {
    if (speed > 100) speed = 99;
    if (speed < -100) speed = -99;

    if (speed > 0) {
        int pwm_value = (speed * __HAL_TIM_GET_AUTORELOAD(motor->htim)) / 100;
        __HAL_TIM_SET_COMPARE(motor->htim, motor->channel1, pwm_value);
        __HAL_TIM_SET_COMPARE(motor->htim, motor->channel2, 0);
    } else if (speed < 0) {
        int pwm_value = (-speed * __HAL_TIM_GET_AUTORELOAD(motor->htim)) / 100;
        __HAL_TIM_SET_COMPARE(motor->htim, motor->channel1, 0);
        __HAL_TIM_SET_COMPARE(motor->htim, motor->channel2, pwm_value);
    } else {
        __HAL_TIM_SET_COMPARE(motor->htim, motor->channel1, 0);
        __HAL_TIM_SET_COMPARE(motor->htim, motor->channel2, 0);
    }
}
