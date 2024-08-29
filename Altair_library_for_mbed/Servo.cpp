#include "Servo.h"

// コンストラクタの実装
Servo::Servo(PinName pin) : pwm(pin) {
    pwm.period_ms(20);  // サーボモーターの周期を20msに設定する
}

// 角度を設定するメソッドの実装
void Servo::write(float angle) {
    // 0度から180度までを0.5msから2.5msのパルス幅に変換する
    float pulsewidth = 0.0005 + (angle / 180.0) * 0.002;
    pwm.pulsewidth(pulsewidth);
}
