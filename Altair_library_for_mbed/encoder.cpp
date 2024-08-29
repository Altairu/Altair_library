#include "encoder.h"

Encoder::Encoder(PinName p1, PinName p2)
    : encoder_count(0), last_count(0), interruptA(p1), interruptB(p2) {
    timer.start();
    setupInterrupts(); // 割り込みを設定
}

void Encoder::setupInterrupts() {
    // 割り込みを設定
    interruptA.rise(callback(this, &Encoder::counterArise));
    interruptA.fall(callback(this, &Encoder::counterAfall));
    interruptB.rise(callback(this, &Encoder::counterBrise));
    interruptB.fall(callback(this, &Encoder::counterBfall));
}

int32_t Encoder::getCount() {
    return encoder_count;
}

float Encoder::getRPS() {
    int32_t delta_count = getCount() - last_count;
    last_count = getCount();

    // 経過時間を取得し、秒単位に変換
    float time_elapsed = timer.elapsed_time().count() / 1e6f;  // microseconds to seconds
    timer.reset();  // タイマーをリセット

    // RPSを計算（例：1回転あたり8092パルスと仮定）
    return (delta_count / 8092.0f) / time_elapsed;
}

void Encoder::reset() {
    encoder_count = 0;
}

void Encoder::counterArise() {
    if (interruptB.read() == 0) {
        encoder_count++;
    } else {
        encoder_count--;
    }
}

void Encoder::counterAfall() {
    if (interruptB.read() == 1) {
        encoder_count++;
    } else {
        encoder_count--;
    }
}

void Encoder::counterBrise() {
    if (interruptA.read() == 1) {
        encoder_count++;
    } else {
        encoder_count--;
    }
}

void Encoder::counterBfall() {
    if (interruptA.read() == 0) {
        encoder_count++;
    } else {
        encoder_count--;
    }
}
