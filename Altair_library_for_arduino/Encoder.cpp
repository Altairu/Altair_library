#include "Encoder.h"

Encoder::Encoder(uint8_t pinA, uint8_t pinB) : encoder_count(0), last_count(0), pinA(pinA), pinB(pinB), last_time(micros()) {
    pinMode(pinA, INPUT);
    pinMode(pinB, INPUT);
    attachInterrupt(digitalPinToInterrupt(pinA), std::bind(&Encoder::counterArise, this), RISING);
    attachInterrupt(digitalPinToInterrupt(pinA), std::bind(&Encoder::counterAfall, this), FALLING);
    attachInterrupt(digitalPinToInterrupt(pinB), std::bind(&Encoder::counterBrise, this), RISING);
    attachInterrupt(digitalPinToInterrupt(pinB), std::bind(&Encoder::counterBfall, this), FALLING);
}

void Encoder::counterArise() {
    if (digitalRead(pinB) == LOW) {
        encoder_count++;
    } else {
        encoder_count--;
    }
}

void Encoder::counterAfall() {
    if (digitalRead(pinB) == HIGH) {
        encoder_count++;
    } else {
        encoder_count--;
    }
}

void Encoder::counterBrise() {
    if (digitalRead(pinA) == HIGH) {
        encoder_count++;
    } else {
        encoder_count--;
    }
}

void Encoder::counterBfall() {
    if (digitalRead(pinA) == LOW) {
        encoder_count++;
    } else {
        encoder_count--;
    }
}

int32_t Encoder::getCount() {
    return encoder_count;
}

float Encoder::getRPS() {
    int32_t delta_count = getCount() - last_count;
    last_count = getCount();
    unsigned long current_time = micros();
    float time_elapsed = (current_time - last_time) / 1e6; // 秒単位に変換
    last_time = current_time;
    return (delta_count / 8192.0) / time_elapsed; // 8092パルス/回転と仮定
}

void Encoder::reset() {
    encoder_count = 0;
}
