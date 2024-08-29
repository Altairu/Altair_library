
# PIDController ライブラリ

## 概要
`PIDController` ライブラリは、PID制御を簡単に実装するためのライブラリです。

## 使用方法

### 初期化

```cpp
#include "Altairlibrary.h"

// PIDゲインとサンプリング時間を設定してPIDControllerを初期化する
PIDController pid(1.0, 0.1, 0.01, 10, 0.01);
```

### パラメータの説明

`PIDController pid(1.0, 0.1, 0.01, 10, 0.01);` では、以下のように各パラメータが設定されています。

- `1.0`: **Kp (比例ゲイン)** - 誤差に対する応答の速さを決定します。大きくすると応答が速くなりますが、オーバーシュートのリスクが増えます。
- `0.1`: **Ki (積分ゲイン)** - 誤差が継続しているときに出力を増加させ、定常偏差を修正します。大きくすると定常偏差が減少しますが、応答が遅くなる可能性があります。
- `0.01`: **Kd (微分ゲイン)** - 誤差の変化率に基づいて出力を減少させ、オーバーシュートを防止します。大きくするとシステムの安定性が向上しますが、過度に大きいとノイズに敏感になります。
- `10`: **time_constant (時定数)** - PID制御の出力に対するローパスフィルタの時定数です。出力の滑らかさを調整します。
- `0.01`: **dt (サンプリング時間)** - 制御ループのサンプリング時間です。制御ループがどのくらいの頻度で実行されるかを決定します。

### メソッド

- `float compute(float setpoint, float measured_value)`: 設定値 (`setpoint`) と測定値 (`measured_value`) に基づいて、PID制御出力を計算します。

- `void reset()`: PIDの内部状態をリセットします。

### サンプルコード

```cpp
#include "mbed.h"
#include "Altairlibrary.h"

PIDController pid(1.0, 0.1, 0.01, 10, 0.01);

int main() {
    float setpoint = 100.0; // 目標値
    float measured_value = 95.0; // 現在の測定値

    while (true) {
        float output = pid.compute(setpoint, measured_value);
        ThisThread::sleep_for(100ms);
    }
}
```

```cpp
#include "mbed.h"
#include "Altairlibrary.h"
#include "rtos.h"

// ピン定義
#define ENCODER_PIN_A PA_1
#define ENCODER_PIN_B PA_0
#define MOTOR_PIN_1 PB_8
#define MOTOR_PIN_2 PB_9

// エンコーダー、モータードライバー、PIDコントローラーのインスタンス作成
Encoder encoder(ENCODER_PIN_A, ENCODER_PIN_B);
MotorDriver motor(MOTOR_PIN_1, MOTOR_PIN_2);
PIDController pid(0.4, 0.05, 0.025, 20, 0.001); // Kp, Ki, Kd, 時定数, サンプリング時間 

Thread control_thread;
float setpoint = 1.0; // 目標RPS

void controlLoop() {
    while (1) {
        int32_t rps = encoder.getRPS(); // 現在の回転速度（RPS）

        float control_signal = pid.compute(setpoint, rps); // PID制御で出力を計算
        float motor_speed = (float)(control_signal); 
        if (motor_speed > 100) motor_speed = 100;
        if (motor_speed < -100) motor_speed = -100;

        motor.setSpeed(motor_speed); // モーター速度制御
    }
}

int main() {
    control_thread.start(controlLoop); // PID制御スレッドを開始

    while (1) {
        // 他のタスクをここで実行
        ThisThread::sleep_for(1s); // 主にメインループが他のことをやっている間、制御ループは別スレッドで動作する
    }
}


```
