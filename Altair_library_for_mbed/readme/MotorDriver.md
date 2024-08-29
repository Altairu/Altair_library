
# MotorDriver ライブラリ

## 概要
`MotorDriver` ライブラリは、DCモーターを制御するためのライブラリです。

## 使用方法

### 初期化

```cpp
#include "Altairlibrary.h"

// ps1Pinとps2PinにはモータードライバのPWMピンを指定して初期化します
MotorDriver motor(PD_5, PD_6);
```

### メソッド

- `void setSpeed(int speed)`: モーターの速度と回転方向を設定します。速度は -100 から 100 の範囲で指定します。
  - `100` はフルスピードで前進
  - `-100` はフルスピードで後退
  - `0` は停止

- `void stop()`: モーターをフリーラン状態で停止させます（慣性による停止）。

- `void brake()`: モーターをショートブレーキを用いて迅速に停止させます。

### サンプルコード

```cpp
#include "mbed.h"
#include "Altairlibrary.h"

MotorDriver motor(PA_6, PA_7);

int main() {
    motor.setSpeed(50);  // モーターを50%の速度で前進
    ThisThread::sleep_for(2s);

    motor.setSpeed(-50); // モーターを50%の速度で後退
    ThisThread::sleep_for(2s);

    motor.setSpeed(0);   // モーターをフリーランで停止（慣性による停止）
    ThisThread::sleep_for(2s);

    motor.stop();        // モーターをフリーランで停止（慣性による停止）
}
```

### 詳細な説明

- `setSpeed(int speed)`: 指定された速度でモーターを制御します。`speed` の値に応じて、モーターが前進・後退し、`0` の場合はショートブレーキで停止します。
- `stop()`: モーターのPWM信号を0に設定し、フリーラン状態で停止させます。この場合、モーターは慣性で徐々に止まります。
- `brake()`: ショートブレーキ機能を使用してモーターを即座に停止させます。PWM信号を強制的にHIGHに設定することで、モーターの端子を短絡し、回転を急速に止めます。

### 注意事項

- 高速回転から急停止させると、モーターやドライバに負荷がかかるため、適切な使用を心がけてください。
