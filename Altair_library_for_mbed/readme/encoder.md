# Encoder ライブラリ（非推奨）
incencを使用することをおすすめする．
## 概要
`Encoder` ライブラリは、エンコーダのパルスを読み取り、回転数 (RPS) や角度 (degrees) を計算するためのライブラリです。割り込み方式（`InterruptIn`）とポーリング方式を選択でき、柔軟にエンコーダの読み取りを行うことができます。

## 特徴

- 割り込み方式 (`INTERRUPT_MODE`) とポーリング方式 (`POLLING_MODE`) を選択可能
- 回転数 (RPS) や角度 (degrees) を計算
- エンコーダのカウントを取得可能

## 使用方法

### 初期化

エンコーダの初期化時に、割り込みモードまたはポーリングモードを指定します。

```cpp
#include "encoder.h"

// 割り込みモードで初期化
Encoder encoder(PA_1, PA_0, INTERRUPT_MODE); 

// ポーリングモードで初期化
Encoder encoder2(PB_3, PB_5, POLLING_MODE); 
```

### メソッド

- `int32_t getCount()`: 現在のエンコーダのカウント値を返します。
- `float getRPS()`: 回転数 (回転/秒) を取得します。
- `int32_t getAngle()`: 現在の角度 (0〜359度) を取得します。
- `void reset()`: カウント値をリセットします。
- `void update()`: ポーリングモードで使用する場合、このメソッドを定期的に呼び出してエンコーダの状態を更新します。

### 割り込みモード (`INTERRUPT_MODE`) とポーリングモード (`POLLING_MODE`)

- 割り込みモード (`INTERRUPT_MODE`): `InterruptIn` を使用して自動的にエンコーダの状態を更新します。
- ポーリングモード (`POLLING_MODE`): `update()` メソッドを定期的に呼び出してエンコーダの状態を手動で更新します。割り込みの競合を避けるために使用します。

### サンプルコード

#### 割り込みモードでの使用例

```cpp
#include "mbed.h"
#include "encoder.h"

Encoder encoder(PA_1, PA_0, INTERRUPT_MODE);

int main() {
    while (true) {
        printf("Count: %ld, RPS: %f, Angle: %ld\n", encoder.getCount(), encoder.getRPS(), encoder.getAngle());
        ThisThread::sleep_for(1s);
    }
}
```

#### ポーリングモードでの使用例

```cpp
#include "mbed.h"
#include "encoder.h"

Encoder encoder(PA_1, PA_0, POLLING_MODE);

void updateEncoders() {
    encoder.update();  // 定期的に呼び出してエンコーダの状態を更新
}

int main() {
    while (true) {
        updateEncoders();  // ポーリングでエンコーダを更新
        printf("Count: %ld, RPS: %f, Angle: %ld\n", encoder.getCount(), encoder.getRPS(), encoder.getAngle());
        ThisThread::sleep_for(10ms);
    }
}
```

#### スレッドを使った制御の例

```cpp
#include "mbed.h"
#include "encoder.h"
#include "MotorDriver.h"
#include "PIDController.h"
#include "rtos.h"

// ピン定義
#define ENCODER_PIN_A PA_1
#define ENCODER_PIN_B PA_0
#define MOTOR_PIN_1 PB_8
#define MOTOR_PIN_2 PB_9

// エンコーダー、モータードライバー、PIDコントローラーのインスタンス作成
Encoder encoder(ENCODER_PIN_A, ENCODER_PIN_B, INTERRUPT_MODE);
MotorDriver motor(MOTOR_PIN_1, MOTOR_PIN_2);
PIDController pid(0.4, 0.05, 0.025, 20, 0.001); // Kp, Ki, Kd, 時定数, サンプリング時間 

Thread control_thread;
float setpoint = 1.0; // 目標RPS

void controlLoop() {
    while (1) {
        float rps = encoder.getRPS(); // 現在の回転速度（RPS）

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
