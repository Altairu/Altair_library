# TwoWheelKinematics ライブラリ

## 概要
`TwoWheelKinematics` ライブラリは、対向2輪型ロボットの運動学を用いて、ロボットの移動速度と旋回角速度からそれぞれのタイヤの速度を計算するためのライブラリです。

ユーザーはロボットのタイヤ直径とタイヤ間距離を設定し、ロボットの速度と旋回角速度を入力することで、右タイヤと左タイヤの速度を取得できます。


## 使用方法

### 1. ライブラリの初期化

まず、タイヤ直径とタイヤ間距離を指定して、`TwoWheelKinematics` クラスを初期化します。

```cpp
#include "Altairlibrary.h"

TwoWheelKinematics kinematics(100.0, 200.0); // タイヤ直径100mm, タイヤ間距離200mm
```

### 2. タイヤの速度を計算

次に、ロボットの移動速度 `v` (mm/s) と旋回角速度 `omega` (rad/s) を指定して、各タイヤの速度を計算します。

```cpp
float v = 300.0;    // ロボットの速度 (mm/s)
float omega = 1.0;  // ロボットの旋回角速度 (rad/s)
float vR, vL;       // 各タイヤの速度 (計算結果が格納される)

kinematics.calculateWheelSpeeds(v, omega, vR, vL);

printf("Right Wheel Speed: %f mm/s\n", vR);
printf("Left Wheel Speed: %f mm/s\n", vL);
```

### 3. サンプルコード

以下は、`TwoWheelKinematics` ライブラリを使用したサンプルコードです。

```cpp
#include "mbed.h"
#include "Altairlibrary.h"

int main() {
    // タイヤ直径 (mm) とタイヤ間距離 (mm) を指定してクラスを初期化
    TwoWheelKinematics kinematics(100.0, 200.0); // 例: タイヤ直径 100mm, タイヤ間距離 200mm

    float v = 300.0;    // ロボットの速度 (mm/s)
    float omega = 1.0;  // ロボットの旋回角速度 (rad/s)
    float vR, vL;       // 各タイヤの速度 (出力される値)

    // 各タイヤの速度を計算
    kinematics.calculateWheelSpeeds(v, omega, vR, vL);

    // 結果を表示
    printf("Right Wheel Speed: %f mm/s\n", vR);
    printf("Left Wheel Speed: %f mm/s\n", vL);

    while (true) {
        // メインループ処理
    }
}
```

### 4. パラメータの説明

- **タイヤ直径 (wheel_diameter)**: タイヤの直径をミリメートル (mm) で指定します。
- **タイヤ間距離 (wheel_distance)**: ロボットの左右のタイヤ間の距離をミリメートル (mm) で指定します。
- **移動速度 (v)**: ロボットの移動速度をミリメートル毎秒 (mm/s) で指定します。
- **旋回角速度 (omega)**: ロボットの旋回角速度をラジアン毎秒 (rad/s) で指定します。

