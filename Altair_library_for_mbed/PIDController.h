#ifndef PID_CONTROLLER_H_
#define PID_CONTROLLER_H_

#include "mbed.h"
#include "rtos.h"

class PIDController {
public:
    PIDController(float Kp, float Ki, float Kd, float time_constant, float dt)
        : Kp(Kp), Ki(Ki), Kd(Kd), time_constant(time_constant), dt(dt),
          prev_error(0), integral(0), prev_output(0) {}

    // PID計算メソッド
    float compute(float setpoint, float measured_value) {
        // 誤差計算
        float error = setpoint - measured_value;

        // 積分項
        integral += error * dt;

        // 微分項
        float derivative = (error - prev_error) / dt;

        // PID計算
        float output = Kp * error + Ki * integral + Kd * derivative;

        // ローパスフィルタ適用
        output = (prev_output * time_constant + output) / (time_constant + dt);

        // 前回の値を更新
        prev_error = error;
        prev_output = output;

        return output;
    }

    void reset() {
        prev_error = 0;
        integral = 0;
        prev_output = 0;
    }

private:
    float Kp, Ki, Kd;        // PIDゲイン
    float time_constant;     // ローパスフィルタの時定数
    float dt;                // サンプリング時間
    float prev_error;        // 前回の誤差
    float integral;          // 積分項
    float prev_output;       // 前回の出力
};

#endif // PID_CONTROLLER_H_
