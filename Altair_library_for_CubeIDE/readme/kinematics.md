# Kinematics Library for STM32

`kinematics`ライブラリは、3輪・4輪のオムニホイールやメカナムホイールのロボット向けの運動学計算を行い、指定された速度ベクトル (Vx, Vy, ω) を各ホイールの速度に変換するためのライブラリです。

## 特徴

- **対応ホイールモード**:
    - **OMNI_3**: 3輪オムニホイール
    - **OMNI_4**: 4輪オムニホイール
    - **MEKANUM**: メカナムホイール

- **入力**: 
    - `Vx` (前後方向の速度)
    - `Vy` (左右方向の速度)
    - `ω` (回転速度)

- **出力**:
    - 各ホイールの目標速度（浮動小数点）

## 構成ファイル

- `kinematics.h`: ライブラリのヘッダーファイル。関数プロトタイプや構造体の定義が含まれます。
- `kinematics.c`: 運動学の計算ロジックを実装したソースファイル。

## インストール

1. **ファイルの追加**: STM32プロジェクトに`kinematics.c`と`kinematics.h`を追加します。
2. **インクルード**: 使用するファイルで`#include "kinematics.h"`を追加し、ライブラリをインクルードします。

## 使用方法

### 1. ライブラリの初期化

最初に、`Kinematics`構造体のインスタンスを作成し、ロボットの各パラメータを設定します。

```c
#include "kinematics.h"

// 運動学インスタンスの初期化
Kinematics kinematics;
kinematics.mode = OMNI_3;            // モードを選択 (OMNI_3, OMNI_4, MEKANUM)
kinematics.robot_diameter = 150.0;   // ロボットの直径 (mm)
kinematics.wheel_radius = 30.0;      // ホイール半径 (mm)
```

### 2. 目標ホイール速度の取得

ロボットの目標速度`Vx`, `Vy`, `ω`から各ホイールの速度を計算します。`Kinematics_GetTargetSpeeds`関数を呼び出し、個々のホイール速度を取得します。

```c
float speedFR, speedFL, speedBR, speedBL; // 各ホイールの速度変数
float Vx = 100.0, Vy = 0.0, omega = 0.5;  // 入力するロボットの目標速度

Kinematics_GetTargetSpeeds(&kinematics, Vx, Vy, omega, &speedFR, &speedFL, &speedBR, &speedBL);
```

- **引数説明**:
    - `&kinematics`: `Kinematics`構造体のポインタ
    - `Vx`: 前後方向の速度 (mm/s)
    - `Vy`: 左右方向の速度 (mm/s)
    - `omega`: 回転速度 (rad/s)
    - `speedFR`, `speedFL`, `speedBR`, `speedBL`: 各ホイールの出力速度の格納場所

### 3. 計算されたホイール速度の使用

出力される`speedFR`, `speedFL`, `speedBR`, `speedBL`は、ホイールの回転速度 (rad/s) です。これを各モーターに設定することで、指定したロボットの速度に基づいて移動できます。

### 使用例

```c
Kinematics kinematics;
kinematics.mode = OMNI_3;
kinematics.robot_diameter = 150.0;
kinematics.wheel_radius = 30.0;

float speedFR, speedFL, speedBR;
float Vx = 100.0;   // 前方向速度
float Vy = 50.0;    // 横方向速度
float omega = 0.2;  // 回転速度

Kinematics_GetTargetSpeeds(&kinematics, Vx, Vy, omega, &speedFR, &speedFL, &speedBR, NULL);

// 各モーターに速度を設定
MotorDriver_setSpeed(&motorFR, (int)speedFR);
MotorDriver_setSpeed(&motorFL, (int)speedFL);
MotorDriver_setSpeed(&motorBR, (int)speedBR);
```
