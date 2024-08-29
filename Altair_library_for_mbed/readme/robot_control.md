
# RobotControl ライブラリ

## 概要

`RobotControl` ライブラリは、Mecanum、Omni3、Omni4 のロボットの各モーターのピン設定、エンコーダのピン設定、各タイヤのPIDゲインを設定し、`Kinematics` を使って計算された目標値を基にロボットを制御します。

## 使用方法

### 1. ライブラリの初期化

ロボットのモード（Mecanum、Omni3、Omni4）を選択し、`RobotControl` オブジェクトを初期化します。車輪の半径と旋回半径、制御モード（RPSまたはmm/s）を指定します。

```cpp
RobotControl robot(Mecanum_Mode, 50.0, 100.0, RPS_MODE); // またはMMPS_MODE
```

### 2. モーターとエンコーダのピン設定

各モーターとエンコーダのピンを個別に設定します。

```cpp
// モーターピン設定
robot.setMotorPin(0, PB_14, PB_15); // モーター0
robot.setMotorPin(1, PA_8, PA_11); // モーター1
robot.setMotorPin(2, PA_6, PA_7); // モーター2
robot.setMotorPin(3, PB_8, PB_9); // モーター3

// エンコーダピン設定
robot.setEncoderPin(0, PA_0, PA_1); // モーター0のエンコーダ
robot.setEncoderPin(1, PB_3, PA_5); // モーター1のエンコーダ
robot.setEncoderPin(2, PB_6, PB_7); // モーター2のエンコーダ
robot.setEncoderPin(3, PC_6, PC_7); // モーター3のエンコーダ
```

### 3. 各タイヤのPIDゲイン設定

各タイヤごとにPIDゲインを個別に設定します。

```cpp
robot.setPIDGains(0, 1.0, 0.1, 0.01, 0.5); // モーター0
robot.setPIDGains(1, 1.2, 0.1, 0.01, 0.5); // モーター1
robot.setPIDGains(2, 1.0, 0.15, 0.02, 0.5); // モーター2
robot.setPIDGains(3, 1.1, 0.1, 0.01, 0.5); // モーター3
```

### 4. ロボットの制御開始

ロボットの目標速度を設定して制御を開始します。この動作はマルチスレッドで行われます。

```cpp
robot.startControl(100.0, 0.0, 30.0); // vx=100 mm/s, vy=0 mm/s, omega=30度/s
```

制御を停止するには `robot.stopControl()` を呼び出します。

## 例

### 例1: Mecanumロボットの制御

```cpp
#include "mbed.h"
#include "Altairlibrary.h"

int main() {
    // RobotControl オブジェクトの初期化
    RobotControl robot(Mecanum_Mode, 50.0, 100.0, RPS_MODE);

    // モーターピン設定
    robot.setMotorPin(0, PB_14, PB_15); // モーター0
    robot.setMotorPin(1, PA_8, PA_11); // モーター1
    robot.setMotorPin(2, PA_6, PA_7); // モーター2
    robot.setMotorPin(3, PB_8, PB_9); // モーター3

    // エンコーダピン設定
    robot.setEncoderPin(0, PA_0, PA_1); // モーター0のエンコーダ
    robot.setEncoderPin(1, PB_3, PA_5); // モーター1のエンコーダ
    robot.setEncoderPin(2, PB_6, PB_7); // モーター2のエンコーダ
    robot.setEncoderPin(3, PC_6, PC_7); // モーター3のエンコーダ

    // PIDゲイン設定
    robot.setPIDGains(0, 1.0, 0.1, 0.01, 0.5); // モーター0
    robot.setPIDGains(1, 1.2, 0.1, 0.01, 0.5); // モーター1
    robot.setPIDGains(2, 1.0, 0.15, 0.02, 0.5); // モーター2
    robot.setPIDGains(3, 1.1, 0.1, 0.01, 0.5); // モーター3

    // ロボット制御開始 (マルチスレッドで実行)
    robot.startControl(100.0, 0.0, 30.0); // vx=100 mm/s, vy=0 mm/s, omega=30度/s

    // 10秒後に制御停止
    ThisThread::sleep_for(10s);
    robot.stopControl();

    while (true) {
        // メインループ
    }
}
```

### 例2: Omni3ロボットの制御

```cpp
#include "mbed.h"
#include "Altairlibrary.h"

int main() {
    // RobotControl オブジェクトの初期化
    RobotControl robot(Omni3_Mode, 50.0, 100.0, RPS_MODE);

    // モーターピン設定
    robot.setMotorPin(0, PB_14, PB_15); // モーター0
    robot.setMotorPin(1, PA_8, PA_11); // モーター1
    robot.setMotorPin(2, PA_6, PA_7); // モーター2

    // エンコーダピン設定
    robot.setEncoderPin(0, PA_0, PA_1); // モーター0のエンコーダ
    robot.setEncoderPin(1, PB_3, PA_5); // モーター1のエンコーダ
    robot.setEncoderPin(2, PB_6, PB_7); // モーター2のエンコーダ

    // PIDゲイン設定
    robot.setPIDGains(0, 1.0, 0.1, 0.01, 0.5); // モーター0
    robot.setPIDGains(1, 1.2, 0.1, 0.01, 0.5); // モーター1
    robot.setPIDGains(2, 1.0, 0.15, 0.02, 0.5); // モーター2

    // ロボット制御開始 (マルチスレッドで実行)
    robot.startControl(100.0, 0.0, 30.0); // vx=100 mm/s, vy=0 mm/s, omega=30度/s

    // 10秒後に制御停止
    ThisThread::sleep_for(10s);
    robot.stopControl();

    while (true) {
        // メインループ
    }
}
```

### 例3: Omni4ロボットの制御

```cpp
#include "mbed.h"
#include "Altairlibrary.h"

int main() {
    // RobotControl オブジェクトの初期化
    RobotControl robot(Omni4_Mode, 50.0, 100.0, RPS_MODE);

    // モーターピン設定
    robot.setMotorPin(0, PB_14, PB_15); // モーター0
    robot.setMotorPin(1, PA_8, PA_11); // モーター1
    robot.setMotorPin(2, PA_6, PA_7); // モーター2
    robot.setMotorPin(3, PB_8, PB_9); // モーター3

    // エンコーダピン設定
    robot.setEncoderPin(0, PA_0, PA_1); // モーター0のエンコーダ
    robot.setEncoderPin(1, PB_3, PA_5); // モーター1のエンコーダ
    robot.setEncoderPin(2, PB_6, PB_7); // モーター2のエンコーダ
    robot.setEncoderPin(3, PC_6, PC_7); // モーター3のエンコーダ

    // PIDゲイン設定
    robot.setPIDGains(0, 

1.0, 0.1, 0.01, 0.5); // モーター0
    robot.setPIDGains(1, 1.2, 0.1, 0.01, 0.5); // モーター1
    robot.setPIDGains(2, 1.0, 0.15, 0.02, 0.5); // モーター2
    robot.setPIDGains(3, 1.1, 0.1, 0.01, 0.5); // モーター3

    // ロボット制御開始 (マルチスレッドで実行)
    robot.startControl(100.0, 0.0, 30.0); // vx=100 mm/s, vy=0 mm/s, omega=30度/s

    // 10秒後に制御停止
    ThisThread::sleep_for(10s);
    robot.stopControl();

    while (true) {
        // メインループ
    }
}
```
