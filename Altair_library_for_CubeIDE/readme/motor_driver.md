# Motor Driver Library Documentation

## 概要
この`motor_driver`ライブラリは、STM32マイコンを使用してDCモーターをPWM制御するためのライブラリです。タイマーのチャンネルを使用してモーターの正転と逆転、速度を制御することができます。

## 初期設定と前提条件

### ハードウェア設定
- モーターはSTM32のGPIOピンとタイマーによるPWM制御を使って駆動します。
- 各モーターには2つのPWM出力が必要です（正転と逆転用）。
- 必要に応じて、`htim`および`TIM_CHANNEL_x`の設定を合わせます。

### 必要なヘッダー
```c
#include "motor_driver.h"
```

## `motor_driver.h` - 関数リスト

### MotorDriver_Init

**説明**  
指定されたタイマーおよびチャンネルを用いてモータードライバを初期化します。

**宣言**
```c
void MotorDriver_Init(MotorDriver *motor, TIM_HandleTypeDef *htimA, uint32_t channelA, TIM_HandleTypeDef *htimB, uint32_t channelB);
```

**パラメータ**
- `motor`: 初期化するモーターの構造体ポインタ。
- `htimA`: 正転用PWM出力のタイマー構造体。
- `channelA`: 正転用PWM出力のタイマーチャンネル。
- `htimB`: 逆転用PWM出力のタイマー構造体。
- `channelB`: 逆転用PWM出力のタイマーチャンネル。

**使用例**
```c
MotorDriver motor;
MotorDriver_Init(&motor, &htim13, TIM_CHANNEL_1, &htim14, TIM_CHANNEL_1);
```

### MotorDriver_setSpeed

**説明**  
指定された速度でモーターを回転させます。速度はパーセントで指定し、負の値は逆回転を示します。

**宣言**
```c
void MotorDriver_setSpeed(MotorDriver *motor, int speed);
```

**パラメータ**
- `motor`: 設定するモーターの構造体ポインタ。
- `speed`: -100〜100の範囲で指定。正の値で正転、負の値で逆転。

**使用例**
```c
MotorDriver_setSpeed(&motor, 50);  // 50%速度で正転
MotorDriver_setSpeed(&motor, -50); // 50%速度で逆転
```

## `motor_driver.c` - 実装の概要

### 構造体

```c
typedef struct {
    TIM_HandleTypeDef *htimA;
    uint32_t channelA;
    TIM_HandleTypeDef *htimB;
    uint32_t channelB;
} MotorDriver;
```

### MotorDriver_Init

この関数は、指定されたタイマーとチャンネルを使ってモーターを初期化します。正転と逆転用のチャンネルを分けることで、モーターの方向を簡単に制御できます。

### MotorDriver_setSpeed

指定したパーセンテージでPWM信号を出力し、モーターの速度と方向を制御します。関数は`speed`の値が正の場合は`htimA`と`channelA`にPWMを出力し、負の場合は`htimB`と`channelB`に出力します。

- `speed`が0の場合、両方のチャンネルが0に設定され、モーターは停止します。

### エラーハンドリング
`Error_Handler()`を利用して、初期化やPWM出力でエラーが発生した場合のデバッグを支援します。

---

## 使用例

### モーター単体テストコード

以下のコードは、モーターが2秒間正転し、1秒停止し、2秒間逆転する動作を繰り返します。

```c
#include "motor_driver.h"

MotorDriver motor;
MotorDriver_Init(&motor, &htim13, TIM_CHANNEL_1, &htim14, TIM_CHANNEL_1);

while (1) {
    MotorDriver_setSpeed(&motor, 50);  // 50%速度で正転
    HAL_Delay(2000);                   // 2秒待機

    MotorDriver_setSpeed(&motor, 0);   // 停止
    HAL_Delay(1000);                   // 1秒待機

    MotorDriver_setSpeed(&motor, -50); // 50%速度で逆転
    HAL_Delay(2000);                   // 2秒待機

    MotorDriver_setSpeed(&motor, 0);   // 停止
    HAL_Delay(1000);                   // 1秒待機
}
```

### トラブルシューティング
- **モーターが動作しない場合**: 
  - `htimA`、`htimB`の設定が正しいことを確認します。
  - PWM出力の設定が正しく、クロックが有効になっていることを確認します。
