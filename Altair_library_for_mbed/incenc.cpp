#include "incenc.h"
#include "stm32f4xx_hal.h"

void IncEnc::init(PinName a_pin, PinName b_pin, TIM_TypeDef *tim, double diameter, int ppr, int period)
{
    // タイマーのクロック有効化
    if (tim == TIM2)
    {
        __HAL_RCC_TIM2_CLK_ENABLE();
    }
    else if (tim == TIM3)
    {
        __HAL_RCC_TIM3_CLK_ENABLE();
    }
    else if (tim == TIM4)
    {
        __HAL_RCC_TIM4_CLK_ENABLE();
    }
    else if (tim == TIM5)
    {
        __HAL_RCC_TIM5_CLK_ENABLE();
    }

    // タイマーとエンコーダー設定
    encoder_handle_.Instance = tim;
    encoder_handle_.Init.Prescaler = 0;
    encoder_handle_.Init.CounterMode = TIM_COUNTERMODE_UP;
    encoder_handle_.Init.Period = 0xFFFF;
    encoder_handle_.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    encoder_init_.EncoderMode = TIM_ENCODERMODE_TI12;
    encoder_init_.IC1Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
    encoder_init_.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    encoder_init_.IC1Prescaler = TIM_ICPSC_DIV1;
    encoder_init_.IC1Filter = 0;
    encoder_init_.IC2Polarity = TIM_INPUTCHANNELPOLARITY_RISING;
    encoder_init_.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    encoder_init_.IC2Prescaler = TIM_ICPSC_DIV1;
    encoder_init_.IC2Filter = 0;

    HAL_TIM_Encoder_Init(&encoder_handle_, &encoder_init_);
    HAL_TIM_Encoder_Start(&encoder_handle_, TIM_CHANNEL_ALL);

    tim->CNT = 30000;

    diameter_ = diameter;
    ppr_ = ppr;
    period_ = period;
    limit_ = 0;
    before_rot_ = 0;
}

int IncEnc::read(TIM_TypeDef *tim)
{
    return (tim->CNT) - 30000;
}

void IncEnc::interrupt(TIM_TypeDef *tim, IncEncData *encoder_data)
{
    int count = read(tim);
    if (count >= 20000 || count <= -20000)
    {
        if (count >= 20000)
        {
            limit_++;
        }
        else
        {
            limit_--;
        }
        reset(tim);
    }
    encoder_data->count = read(tim) + limit_ * 20000;

    // 回転数と角度を計算
    encoder_data->rot = (encoder_data->count) / (double)ppr_;
    encoder_data->deg = encoder_data->rot * 360.0;
    encoder_data->distance = encoder_data->rot * (3.14159 * diameter_);

    // RPSを計算
    encoder_data->rps = (encoder_data->rot - before_rot_) / ((double)period_ * 0.001);
    before_rot_ = encoder_data->rot;

    // 速度を計算
    encoder_data->velocity = encoder_data->rps * 3.14159 * diameter_;
}

void IncEnc::reset(TIM_TypeDef *tim)
{
    tim->CNT = 30000;
}

double IncEnc::get_degrees(IncEncData *encoder_data)
{
    return encoder_data->deg;
}

double IncEnc::get_rps(IncEncData *encoder_data)
{
    return encoder_data->rps;
}
