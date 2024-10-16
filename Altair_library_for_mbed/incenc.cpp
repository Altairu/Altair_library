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

    // GPIOの設定
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = (1 << (a_pin & 0xF)) | (1 << (b_pin & 0xF));
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;

    if (tim == TIM2 || tim == TIM5)
    {
        if (a_pin == PA_5 || b_pin == PA_5)
        {
            __HAL_RCC_GPIOA_CLK_ENABLE();
            GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
            HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
        }
        if (a_pin == PB_3 || b_pin == PB_3)
        {
            __HAL_RCC_GPIOB_CLK_ENABLE();
            GPIO_InitStruct.Alternate = GPIO_AF1_TIM2;
            HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
        }
    }
    else if (tim == TIM3)
    {
        __HAL_RCC_GPIOC_CLK_ENABLE();
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM3;
        HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
    }
    else if (tim == TIM4)
    {
        __HAL_RCC_GPIOB_CLK_ENABLE();
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM4;
        HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    }
    else if (tim == TIM5)
    {
        __HAL_RCC_GPIOA_CLK_ENABLE();
        GPIO_InitStruct.Alternate = GPIO_AF2_TIM5;
        HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
    }

    // タイマーの初期化
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

    // カウンタの初期化
    tim->CNT = 30000;

    diameter_ = diameter;
    ppr_ = ppr;
    period_ = period;
    limit_ = 0;
    before_rot_ = 0;
}

int IncEnc::getCount(TIM_TypeDef *tim)
{
    return (tim->CNT) - 30000;
}

void IncEnc::getEncoderData(TIM_TypeDef *tim, IncEncData *encoder_data)
{
    int count = getCount(tim);
    encoder_data->count = count;

    encoder_data->rot = (encoder_data->count) / (double)ppr_;
    encoder_data->deg = encoder_data->rot * 360.0;
    encoder_data->distance = encoder_data->rot * (3.14159265359 * diameter_);

    encoder_data->rps = (encoder_data->rot - before_rot_) / ((double)period_ * 0.001);
    before_rot_ = encoder_data->rot;
    encoder_data->velocity = encoder_data->rps * 3.14159265359 * diameter_;
}

void IncEnc::reset(TIM_TypeDef *tim)
{
    tim->CNT = 30000;
    limit_ = 0;
    before_rot_ = 0;
}
