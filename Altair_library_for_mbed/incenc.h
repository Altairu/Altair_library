#ifndef INCENC_H_
#define INCENC_H_

#include "mbed.h"

struct IncEncData
{
    int count;
    double rot;      // 回転数
    double deg;      // 角度 (degrees)
    double distance; // 距離
    double velocity; // 速度
    double rps;      // 回転数毎秒 (RPS)
};

class IncEnc
{
public:
    void init(PinName a_pin, PinName b_pin, TIM_TypeDef *tim, double diameter = 100, int ppr = 8192, int period = 1);
    int read(TIM_TypeDef *tim);
    void interrupt(TIM_TypeDef *tim, IncEncData *encoder_data);
    void reset(TIM_TypeDef *tim);

    // 角度を取得
    double get_degrees(IncEncData *encoder_data);

    // 回転数毎秒（RPS）を取得
    double get_rps(IncEncData *encoder_data);

private:
    TIM_HandleTypeDef encoder_handle_;
    TIM_Encoder_InitTypeDef encoder_init_;
    TIM_MasterConfigTypeDef encoder_mas_config_;
    double diameter_;
    int ppr_;           // Pulses per Revolution
    int period_;        // 時間間隔 (ms単位)
    int limit_;         // エンコーダーのオーバーフロー/アンダーフロー補正
    double before_rot_; // 前回の回転数
};

#endif /* INCENC_H_ */
