#ifndef INCENC_H_
#define INCENC_H_

#include "mbed.h"

struct IncEncData
{
    int count;       // エンコーダーカウント
    double rot;      // 回転数
    double deg;      // 角度 (度)
    double distance; // 距離 (例えば、円周距離)
    double velocity; // 速度
    double rps;      // 回転速度 (RPS)
};

class IncEnc
{
public:
    // 初期化
    void init(PinName a_pin, PinName b_pin, TIM_TypeDef *tim, double diameter = 100, int ppr = 8192, int period = 1);

    // カウントの読み取り
    int getCount(TIM_TypeDef *tim);

    // 回転速度（RPS）、角度、距離などのデータを取得
    void getEncoderData(TIM_TypeDef *tim, IncEncData *encoder_data);

    // リセット
    void reset(TIM_TypeDef *tim);

private:
    TIM_HandleTypeDef encoder_handle_;
    TIM_Encoder_InitTypeDef encoder_init_;
    TIM_MasterConfigTypeDef encoder_mas_config_;
    double diameter_;
    int ppr_;
    int period_;
    int limit_;
    double before_rot_;
};

#endif /* INCENC_H_ */
