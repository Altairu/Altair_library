
# Kinematics ライブラリ

## 概要

`Kinematics` ライブラリは、ロボットの運動学的制御をサポートするライブラリです。このライブラリを使用することで、四輪メカナムホイール、三輪オムニホイール、四輪オムニホイールを持つロボットの制御を簡単に行うことができます。ユーザーは制御データの単位として、回転数（RPS）または移動速度（mm/s）を選択できます。

## 機能

- **四輪メカナムホイールの制御** (`Mecanum` クラス)
- **三輪オムニホイールの制御** (`Omni3` クラス)
- **四輪オムニホイールの制御** (`Omni4` クラス)

## 使用方法

### 1. ライブラリの初期化

各ロボットの運動学的制御クラス (`Mecanum`, `Omni3`, `Omni4`) を初期化する際、車輪の半径、旋回半径、制御モードを指定します。

```cpp
#include "Kinematics.h"

ControlMode mode = RPS_MODE; // または MMPS_MODE

Mecanum mecanum(50.0, 100.0, mode); // 車輪半径50mm, 旋回半径100mm, RPSモード
Omni3 omni3(50.0, 100.0, mode);     // 三輪オムニ
Omni4 omni4(50.0, 100.0, mode);     // 四輪オムニ
```

### 2. 制御計算

ロボットの移動速度 (`vx`, `vy`) と回転速度 (`omega`) を入力として、各モータの制御データを計算します。

```cpp
MotorControlData motor_control_data;

// 四輪メカナムホイールの制御
mecanum.calc(100.0, 0.0, 30.0, motor_control_data); // vx=100.0 mm/s, vy=0.0 mm/s, omega=30度/s

// 三輪オムニホイールの制御
omni3.calc(100.0, 100.0, 30.0, motor_control_data);

// 四輪オムニホイールの制御
omni4.calc(100.0, 100.0, 30.0, motor_control_data);
```

### 3. 制御データの使用

計算された各モータの制御データ（RPSまたはmm/s）は、`MotorControlData` 構造体に格納されます。

```cpp
for (int i = 0; i < 4; i++) {
    printf("Motor %d: %f\n", i, motor_control_data.motor_data[i].target_value);
}
```

### 4. クラスの詳細

#### `Mecanum` クラス

- **目的**: 四輪メカナムホイールロボットの運動学的制御を計算します。
- **初期化**:
  - `Mecanum(double wheel_radius, double turning_radius, ControlMode mode)`
  - `wheel_radius`: 車輪の半径 (mm)
  - `turning_radius`: 旋回半径 (mm)
  - `mode`: 制御モード（RPSまたはMMPS）

#### `Omni3` クラス

- **目的**: 三輪オムニホイールロボットの運動学的制御を計算します。
- **初期化**:
  - `Omni3(double wheel_radius, double turning_radius, ControlMode mode)`
  - `wheel_radius`: 車輪の半径 (mm)
  - `turning_radius`: 旋回半径 (mm)
  - `mode`: 制御モード（RPSまたはMMPS）

#### `Omni4` クラス

- **目的**: 四輪オムニホイールロボットの運動学的制御を計算します。
- **初期化**:
  - `Omni4(double wheel_radius, double turning_radius, ControlMode mode)`
  - `wheel_radius`: 車輪の半径 (mm)
  - `turning_radius`: 旋回半径 (mm)
  - `mode`: 制御モード（RPSまたはMMPS）

### 5. 制御モード

- **RPS_MODE**: 各モータの制御データが回転数（RPS）で計算されます。
- **MMPS_MODE**: 各モータの制御データが移動速度（mm/s）で計算されます。


###  Mecanum ホイール配置

Mecanum ホイールロボットでは、4つのホイールがそれぞれの角に配置されます。モーターの番号は以下のように設定されています。

```
モーター配置（Mecanum）

Front
  0         1
|---|     |---|
     \   /
     /   \
|---|     |---|
  3         2
Back
```

- モーター 0: 左前 (Front Left)
- モーター 1: 右前 (Front Right)
- モーター 2: 右後 (Back Right)
- モーター 3: 左後 (Back Left)

###  Omni3 ホイール配置

Omni3 ホイールロボットでは、3つのホイールが120度の間隔で配置されます。モーターの番号は以下のように設定されています。

```
モーター配置（Omni3）

Front
    Front
      ↑
      0

   /       \
  /         \
 /           \
/             \
2             1


```

- モーター 0: 前 (Front)
- モーター 1: 左後 (Back Left)
- モーター 2: 右後 (Back Right)

###  Omni4 ホイール配置

Omni4 ホイールロボットでは、4つのホイールがそれぞれの角に配置されます。モーターの番号は以下のように設定されています。

```
モーター配置（Omni4）

Front
  0         1
|-/-|     |-\-|
     |   |
     |   |
|-\-|     |-/-|
  3         2
Back
```

- モーター 0: 左前 (Front Left)
- モーター 1: 右前 (Front Right)
- モーター 2: 右後 (Back Right)
- モーター 3: 左後 (Back Left)
