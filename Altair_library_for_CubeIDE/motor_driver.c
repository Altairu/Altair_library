#include "motor_driver.h"

// デフォルトPWM周波数を上書き可能なグローバル変数
// ユーザー側で強い定義
//   uint32_t g_motor_driver_default_pwm_hz = 200;
// を用意すると、その値が使用される
__attribute__((weak)) uint32_t g_motor_driver_default_pwm_hz = MOTOR_DRIVER_DEFAULT_PWM_HZ;

// タイマクロックを取得する内部関数
static uint32_t MotorDriver_GetTimerClock(TIM_HandleTypeDef *htim)
{
    RCC_ClkInitTypeDef clkconfig;
    uint32_t flashLatency;
    uint32_t pclk;

    HAL_RCC_GetClockConfig(&clkconfig, &flashLatency);

    // APB2 側のタイマ (TIM1, TIM8, TIM9, TIM10, TIM11)
    if (htim->Instance == TIM1 || htim->Instance == TIM8 ||
        htim->Instance == TIM9 || htim->Instance == TIM10 ||
        htim->Instance == TIM11)
    {
        pclk = HAL_RCC_GetPCLK2Freq();
        if (clkconfig.APB2CLKDivider != RCC_HCLK_DIV1)
        {
            return pclk * 2U;  // タイマクロックは PCLKx の 2 倍
        }
        else
        {
            return pclk;
        }
    }
    // APB1 側のタイマ (TIM2, TIM3, TIM4, TIM5, TIM6, TIM7, TIM12, TIM13, TIM14 など)
    else
    {
        pclk = HAL_RCC_GetPCLK1Freq();
        if (clkconfig.APB1CLKDivider != RCC_HCLK_DIV1)
        {
            return pclk * 2U;
        }
        else
        {
            return pclk;
        }
    }
}

// 1 つのタイマを指定周波数に設定する内部関数
static HAL_StatusTypeDef MotorDriver_ConfigTimerFrequency(TIM_HandleTypeDef *htim, uint32_t frequency_hz)
{
    if (frequency_hz == 0U)
    {
        return HAL_ERROR;
    }

    uint32_t tim_clk = MotorDriver_GetTimerClock(htim);
    if (tim_clk == 0U)
    {
        return HAL_ERROR;
    }

    // ARR が 16bit に収まるようにプリスケーラを計算
    uint32_t prescaler = tim_clk / (frequency_hz * 65536UL);
    if (prescaler > 0U)
    {
        prescaler -= 1U;  // レジスタ値は (PSC+1) なので 1 を引く
    }
    if (prescaler > 0xFFFFU)
    {
        prescaler = 0xFFFFU;
    }

    uint32_t timer_freq = tim_clk / (prescaler + 1U);
    uint32_t arr = (timer_freq / frequency_hz) - 1U;
    if (arr > 0xFFFFU)
    {
        arr = 0xFFFFU;
    }

    // タイマ設定の反映
    __HAL_TIM_DISABLE(htim);
    __HAL_TIM_SET_PRESCALER(htim, prescaler);
    __HAL_TIM_SET_AUTORELOAD(htim, arr);
    __HAL_TIM_SET_COUNTER(htim, 0U);
    __HAL_TIM_ENABLE(htim);

    htim->Init.Prescaler = prescaler;
    htim->Init.Period    = arr;

    return HAL_OK;
}

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

#ifndef MOTOR_DRIVER_DISABLE_DEFAULT_PWM
    // デフォルトで PWM 周波数を g_motor_driver_default_pwm_hz [Hz] に設定
    // 優先度: ユーザー定義変数 > MOTOR_DRIVER_DEFAULT_PWM_HZ マクロ
    MotorDriver_setPwmFrequency(motor, g_motor_driver_default_pwm_hz);
#endif
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

// ショートブレーキ（両ch をフル Duty に設定）
void MotorDriver_Stop(MotorDriver *motor) {
    uint32_t arr = __HAL_TIM_GET_AUTORELOAD(motor->htimA);
    __HAL_TIM_SET_COMPARE(motor->htimA, motor->channelA, arr);
    __HAL_TIM_SET_COMPARE(motor->htimB, motor->channelB, arr);
}

// MotorDriver で使用するタイマの PWM 周波数を設定する関数
// 例: MotorDriver_setPwmFrequency(&motor, 980);  // 約 980 Hz に設定
HAL_StatusTypeDef MotorDriver_setPwmFrequency(MotorDriver *motor, uint32_t frequency_hz)
{
    HAL_StatusTypeDef status;

    status = MotorDriver_ConfigTimerFrequency(motor->htimA, frequency_hz);
    if (status != HAL_OK)
    {
        return status;
    }

    status = MotorDriver_ConfigTimerFrequency(motor->htimB, frequency_hz);
    return status;
}
