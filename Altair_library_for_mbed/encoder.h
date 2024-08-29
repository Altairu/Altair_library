#ifndef ENCODER_H
#define ENCODER_H

#include "mbed.h"

class Encoder {
public:
    Encoder(PinName pinA, PinName pinB); // エンコーダの初期化

    int32_t getCount(); // カウント値を取得
    float getRPS(); // 回転速度 (RPS) を取得
    void reset(); // カウントをリセット

private:
    void setupInterrupts(); // 割り込みを設定
    void counterArise(); // A相の立ち上がり時にカウント
    void counterAfall(); // A相の立ち下がり時にカウント
    void counterBrise(); // B相の立ち上がり時にカウント
    void counterBfall(); // B相の立ち下がり時にカウント

    volatile int32_t encoder_count; // エンコーダのカウント値
    int32_t last_count; // 前回のカウント値
    Timer timer; // 時間計測用タイマー
    InterruptIn interruptA; // A相の割り込み
    InterruptIn interruptB; // B相の割り込み
};

#endif // ENCODER_H
