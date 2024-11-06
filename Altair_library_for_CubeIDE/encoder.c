#include "encoder.h"

void Encoder_Init(Encoder* encoder, TIM_HandleTypeDef* htim, double diameter, int ppr, int period)
{
    encoder->htim = htim;
    encoder->ppr = ppr;
    encoder->diameter = diameter;
    encoder->period = period;
    encoder->limit = 0;
    encoder->before_rot = 0.0;

    encoder->htim->Init.Prescaler = 0;
    encoder->htim->Init.CounterMode = TIM_COUNTERMODE_UP;
    encoder->htim->Init.Period = 65535;
    encoder->htim->Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;

    TIM_Encoder_InitTypeDef encoder_init;
    encoder_init.EncoderMode = TIM_ENCODERMODE_TI12;
    encoder_init.IC1Polarity = TIM_ICPOLARITY_RISING;
    encoder_init.IC1Selection = TIM_ICSELECTION_DIRECTTI;
    encoder_init.IC1Prescaler = TIM_ICPSC_DIV1;
    encoder_init.IC1Filter = 0;
    encoder_init.IC2Polarity = TIM_ICPOLARITY_RISING;
    encoder_init.IC2Selection = TIM_ICSELECTION_DIRECTTI;
    encoder_init.IC2Prescaler = TIM_ICPSC_DIV1;
    encoder_init.IC2Filter = 0;

    HAL_TIM_Encoder_Init(htim, &encoder_init);
    HAL_TIM_Encoder_Start(htim, TIM_CHANNEL_ALL);
    __HAL_TIM_SET_COUNTER(htim, 0);  // 初期値をゼロに設定
}

int Encoder_Read(Encoder* encoder)
{
    return __HAL_TIM_GET_COUNTER(encoder->htim);
}

void Encoder_Interrupt(Encoder* encoder, EncoderData* encoder_data)
{
    int count = Encoder_Read(encoder);

    encoder_data->count = count;
    encoder_data->rot = count / (double)encoder->ppr;
    encoder_data->deg = encoder_data->rot * 360.0;
    encoder_data->distance = encoder_data->rot * (PI * encoder->diameter);

    encoder_data->rps = (encoder_data->rot - encoder->before_rot) / (encoder->period * 0.001);
    encoder_data->velocity = encoder_data->rps * PI * encoder->diameter;

    encoder->before_rot = encoder_data->rot;
}

void Encoder_Reset(Encoder* encoder)
{
    __HAL_TIM_SET_COUNTER(encoder->htim, 0);
}
