
# 自己位置推定ライブラリ

このライブラリは、Omni3およびOmni4のロボットに対応した自己位置推定機能を提供します。エンコーダピン、ホイール角度、ロボット中心からの距離、ホイール直径を設定することで、ロボットの現在位置（x, y）および角度（θ）をリアルタイムで推定することができます。

## 内容

- Omni3およびOmni4のロボットに対応
- 各ホイールの角度を「正面を0度として時計回りに増加する」形式で設定可能
- ホイールの速度と角度を基にロボットの速度（vx, vy, ω）を計算し、位置と角度を推定
- 各ホイールの角度、ロボット中心からの距離、ホイール直径を設定可能
- **POLLING_MODE** と **INTERRUPT_MODE** に対応したエンコーダ設定

## 使い方

### 1. ライブラリの初期化

ロボットのモード（Omni3またはOmni4）を選択し、ライブラリを初期化します。初期化時にはホイール半径やロボット半径を指定する必要はありません。また、エンコーダのモード（POLLING_MODE または INTERRUPT_MODE）を指定します。

```cpp
RobotLocalization robot(Omni4_Mode, POLLING_MODE); // Omni4モードで初期化、POLLING_MODEを使用
```

### 2. エンコーダピンと角度、距離、ホイール直径の設定

各モーターのエンコーダピン、ホイール角度、ロボット中心からの距離、ホイール直径を設定します。角度は、正面を0度として時計回りに増加する形式で設定します。

```cpp
robot.setEncoderPinsAndProperties(0, PA_0, PA_1, 0.0, 100.0, 50.0);   // 正面（0度）、距離100mm、ホイール直径50mm
robot.setEncoderPinsAndProperties(1, PB_3, PA_5, 90.0, 100.0, 50.0);  // 右側（90度）、距離100mm、ホイール直径50mm
robot.setEncoderPinsAndProperties(2, PB_6, PB_7, 180.0, 100.0, 50.0); // 背面（180度）、距離100mm、ホイール直径50mm
robot.setEncoderPinsAndProperties(3, PC_6, PC_7, 270.0, 100.0, 50.0); // 左側（270度）、距離100mm、ホイール直径50mm
```

### 3. 自己位置推定の開始

自己位置推定を開始します。この関数はスレッド内で実行され、継続的に位置を更新します。

```cpp
robot.estimatePosition();
```

### 4. 現在位置の取得

現在のロボットの位置（x, y）と角度（θ）を取得します。

```cpp
double x, y, theta;
robot.getPosition(x, y, theta);
printf("x: %f, y: %f, theta: %f\n", x, y, theta);
```

### 5. サンプルコード

以下に、上記のステップをまとめた完全なコード例を示します。

```cpp
#include "mbed.h"
#include "inverse_kinematics.h"  

int main() {
    // Omni4モードで自己位置推定を行う（POLLING_MODEを使用）
    RobotLocalization robot(Omni4_Mode, POLLING_MODE);

    // エンコーダのピンとプロパティ設定（モーター番号、Aピン、Bピン、角度、距離、ホイール直径を指定）
    robot.setEncoderPinsAndProperties(0, PA_0, PA_1, 0.0, 100.0, 50.0);   // 正面（0度）、距離100mm、ホイール直径50mm
    robot.setEncoderPinsAndProperties(1, PB_3, PA_5, 90.0, 100.0, 50.0);  // 右側（90度）、距離100mm、ホイール直径50mm
    robot.setEncoderPinsAndProperties(2, PB_6, PB_7, 180.0, 100.0, 50.0); // 背面（180度）、距離100mm、ホイール直径50mm
    robot.setEncoderPinsAndProperties(3, PC_6, PC_7, 270.0, 100.0, 50.0); // 左側（270度）、距離100mm、ホイール直径50mm

    // 自己位置推定開始
    robot.estimatePosition();

    // メインループで現在位置を表示
    while (true) {
        double x, y, theta;
        robot.getPosition(x, y, theta);
        printf("x: %f, y: %f, theta: %f\n", x, y, theta);
        ThisThread::sleep_for(500ms);
    }
}
```
