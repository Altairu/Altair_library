#ifndef MOTOR_DRIVER_H
#define MOTOR_DRIVER_H

#include "mbed.h"

class MotorDriver {
public:
    MotorDriver(PinName ps1Pin, PinName ps2Pin) : ps1(ps1Pin), ps2(ps2Pin) {
        ps1.period(0.001f); // PWM周期を1msに設定
        ps2.period(0.001f); // PWM周期を1msに設定
    }

    void setSpeed(int speed) {
        if (speed > 100) speed = 100;
        if (speed < -100) speed = -100;

        if (speed > 0) {
            forward(speed / 100.0f);
        } else if (speed < 0) {
            reverse(-speed / 100.0f);
        } else {
            stop();
        }
    }

private:
    void stop() {
        ps1.write(0.0f);
        ps2.write(0.0f);
    }

    void brake() {
        // ショートブレーキの実装
        ps1.write(1.0f); // 両方のPWMピンをHIGHに設定
        ps2.write(1.0f); // これにより、モーターはすぐに停止
    }

    void forward(float speed) {
        if (speed > 0.95f) speed = 0.95f;
        ps1.write(speed);
        ps2.write(0.0f);
    }

    void reverse(float speed) {
        if (speed > 0.95f) speed = 0.95f;
        ps1.write(0.0f);
        ps2.write(speed);
    }

    PwmOut ps1;
    PwmOut ps2;
};

#endif // MOTOR_DRIVER_H
