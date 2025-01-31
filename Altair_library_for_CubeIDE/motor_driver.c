#include "motor_driver.h"

// 初期化関数
void MotorDriver_Init(MotorDriver* motor, TIM_HandleTypeDef* htimA, uint32_t channelA,
                      TIM_HandleTypeDef* htimB, uint32_t channelB) {
    motor->htimA = htimA;
/*
motor->htimA = htimA;
motor は MotorDriver 型のポインタ。
motor->htimA は MotorDriver 構造体のメンバ変数 htimA を指す。
htimA は関数の引数で渡された TIM_HandleTypeDef* 型の変数。
motor->htimA = htimA; は htimA のポインタ値を motor->htimA に代入する。

-> は 構造体ポインタが指すメンバにアクセスする 演算子。
MotorDriver 構造体のメンバを関数外でも参照・操作できるようにするために、ポインタを渡している。
htimA や htimB は タイマハンドルのポインタ なので、直接コピーするとアドレスが共有される（実体のコピーではない）。
channelA や channelB は 数値データ（uint32_t） なので、値がコピーされる。

関数の引数から受け取ったポインタと数値を構造体のメンバ変数に代入して、後で使えるようにする処理
*/
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
