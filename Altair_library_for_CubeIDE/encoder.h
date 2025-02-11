#ifndef ENCODER_H_
#define ENCODER_H_

#include "stm32f4xx_hal.h"
#include <math.h>

#define PI 3.14159265359

typedef struct
{
    int count;
    double rot;
    double deg;
    double distance;
    double velocity;
    double rps;
} EncoderData;

typedef struct
{
    TIM_HandleTypeDef *htim; // タイマーのハンドル
    int ppr;                 // エンコーダのパルス数
    double diameter;         // エンコーダ接続ホイールの直径
    int period;
    int limit;
    double before_rot;
    double before_deg;
} Encoder;

void Encoder_Init(Encoder *encoder, TIM_HandleTypeDef *htim, double diameter, int ppr, int period);
int Encoder_Read(Encoder *encoder);
void Encoder_Interrupt(Encoder *encoder, EncoderData *encoder_data);
void Encoder_Reset(Encoder *encoder);

#endif /* ENCODER_H_ */
