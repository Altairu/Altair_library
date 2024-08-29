#ifndef SERVO_H
#define SERVO_H

#include "mbed.h"

class Servo {
public:
    // コンストラクタでピンを設定する
    Servo(PinName pin);

    // 角度を設定するメソッド
    void write(float angle);

private:
    PwmOut pwm;
};

#endif // SERVO_H
