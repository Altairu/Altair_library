# Motor Driver Library Documentation

`motor_driver.h`および`motor_driver.c`は、STM32マイコンを用いてDCモーターの速度と回転方向をPWM信号で制御するためのライブラリです。このライブラリは、PWM出力を利用してモーターに必要な電圧や回転方向を設定し、正転、逆転、および速度調整を行います。

## ファイル構成

- `motor_driver.h`: モータードライバライブラリのインターフェースを定義。
- `motor_driver.c`: ライブラリの実装を提供。

## 機能

- モーターの初期化
- モーターの回転速度と方向の制御
- モーターの停止

## 構造体

### MotorDriver

`MotorDriver`は、PWM信号でモーターを制御するために必要な情報を保持する構造体です。

```c
typedef struct {
    TIM_HandleTypeDef *htim; // PWM用タイマーのハンドル
    uint32_t channel1;       // PWM出力のチャネル1
    uint32_t channel2;       // PWM出力のチャネル2
} MotorDriver;
```

- `htim`: モーター制御に使用するタイマーのハンドル。
- `channel1`: 正転に使用するPWMチャネル。
- `channel2`: 逆転に使用するPWMチャネル。

## 関数

### MotorDriver_Init

```c
void MotorDriver_Init(MotorDriver *motor, TIM_HandleTypeDef *htim, uint32_t channel1, uint32_t channel2);
```

#### 説明
モーターの初期化を行います。この関数は、PWMチャネルを使用してモーターの回転方向と速度を制御するために必要な設定を行います。

#### パラメータ
- `motor`: 初期化するモータードライバ構造体へのポインタ。
- `htim`: 使用するタイマーのハンドル。
- `channel1`: 正転用PWMチャネル。
- `channel2`: 逆転用PWMチャネル。

#### 使用例
```c
MotorDriver motor;
MotorDriver_Init(&motor, &htim3, TIM_CHANNEL_1, TIM_CHANNEL_2);
```

---

### MotorDriver_setSpeed

```c
void MotorDriver_setSpeed(MotorDriver *motor, int speed);
```

#### 説明
モーターの回転速度と方向を設定します。正の速度で正転、負の速度で逆転、0で停止させます。

#### パラメータ
- `motor`: 制御するモータードライバ構造体へのポインタ。
- `speed`: モーターの回転速度（範囲 -100 ～ 100）。

#### 使用例
```c
// モーターを50%の速度で正転
MotorDriver_setSpeed(&motor, 50);

// モーターを50%の速度で逆転
MotorDriver_setSpeed(&motor, -50);

// モーターを停止
MotorDriver_setSpeed(&motor, 0);
```

---

## 使用方法

### 初期設定

1. `motor_driver.h`と`motor_driver.c`をプロジェクトに追加します。
2. 使用するPWM用のタイマーをSTM32CubeMXで設定し、チャネルを選択してPWMモードに設定します。
3. コード内で`MotorDriver`構造体を宣言し、`MotorDriver_Init`関数で初期化します。

### モーターの速度制御

`MotorDriver_setSpeed`関数を用いて、モーターの速度と回転方向を指定します。`speed`パラメータに正の値を設定すると正転、負の値で逆転、0で停止します。

### 使用例

以下に、基本的な初期化と制御の使用例を示します。

```c
#include "motor_driver.h"

int main(void)
{
    // 初期化コード（HAL初期化、クロック設定、タイマー初期化など）

    MotorDriver motor;
    MotorDriver_Init(&motor, &htim3, TIM_CHANNEL_1, TIM_CHANNEL_2);

    while (1) {
        // 50%の速度で正転
        MotorDriver_setSpeed(&motor, 50);
        HAL_Delay(2000);  // 2秒待機

        // 停止
        MotorDriver_setSpeed(&motor, 0);
        HAL_Delay(1000);  // 1秒待機

        // 50%の速度で逆転
        MotorDriver_setSpeed(&motor, -50);
        HAL_Delay(2000);  // 2秒待機

        // 停止
        MotorDriver_setSpeed(&motor, 0);
        HAL_Delay(1000);  // 1秒待機
    }
}
```

## 注意事項

- PWM周期に気をつけてください。