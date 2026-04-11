# Motor Driver Library Documentation

## 概要
`motor_driver` ライブラリは、STM32 マイコンを使用して DC モーターを PWM 制御するためのライブラリです。2 本の PWM チャンネル（正転側 / 逆転側）を使ってモーターの方向と速度を制御します。

## 初期設定と前提条件

### ハードウェア設定

- モーターは STM32 の GPIO ピンとタイマ PWM 出力を使って駆動します。
- 1 つのモーターにつき 2 つの PWM 出力が必要です（正転用 / 逆転用）。
- `htim` と `TIM_CHANNEL_x` が、実際に PWM が出ているタイマ / チャンネルに対応している必要があります。

### 必要なヘッダー
```c
#include "motor_driver.h"
```

---

## PWM 周波数に関する仕様

### デフォルト動作（何も設定しない場合　ALTAIR_MD_V７の時）

ヘッダで何もマクロを定義していない場合、`MotorDriver_Init` を呼ぶだけで、モータに紐づく 2 つのタイマ（`htimA` / `htimB`）の PWM 周波数が **自動的に約 980 Hz** に設定されます。

- デフォルト値はマクロ `MOTOR_DRIVER_DEFAULT_PWM_HZ` で定義されています。
  - 既定値: `920U`
- ライブラリ内部で `HAL_RCC_GetPCLKxFreq()` を使ってタイマクロックを取得し、PSC/ARR を計算して設定します。

```c
// motor_driver.h 内（抜粋）
#ifndef MOTOR_DRIVER_DEFAULT_PWM_HZ
#define MOTOR_DRIVER_DEFAULT_PWM_HZ 980U
#endif
```

### 自動 980 Hz 固定を無効化したい場合

自前で PSC/ARR を設定したい場合は、Altair ライブラリをインクルードする前に

```c
#define MOTOR_DRIVER_DISABLE_DEFAULT_PWM
#include "Altair_library_for_CubeIDE/altair.h"
```

のように `MOTOR_DRIVER_DISABLE_DEFAULT_PWM` を定義してください。これにより、`MotorDriver_Init` 内で行われる自動周波数設定が無効になります。

### デフォルト周波数を変更したい場合　ALTAIR_MD_V8の時（200Hz）

プロジェクト全体で「デフォルトは 200 Hz にしたい」などの場合は、次のいずれかの方法を使います。

1. **マクロで指定（ヘッダインクルード前）**

  ```c
  #define MOTOR_DRIVER_DEFAULT_PWM_HZ 200U
  #include "Altair_library_for_CubeIDE/altair.h"
  ```

2. **グローバル変数で指定（どこか1つのCファイルに定義）**

  ```c
  // motor_driver.c には weak 定義
  //   __attribute__((weak)) uint32_t g_motor_driver_default_pwm_hz = MOTOR_DRIVER_DEFAULT_PWM_HZ;
  // があるため、ユーザ側で同名の変数を定義するとこちらが優先される。
  uint32_t g_motor_driver_default_pwm_hz = 200U;  // 200Hz
  ```

### ランタイムで任意の周波数に変更する

ヘッダには、任意の周波数に変更する関数も用意されています。

```c
HAL_StatusTypeDef MotorDriver_setPwmFrequency(MotorDriver *motor, uint32_t frequency_hz);
```

- `frequency_hz`: 設定したい PWM 周波数 [Hz]
- 戻り値: `HAL_OK` で成功、それ以外でエラー

自動 920 Hz を無効化した状態でも、この関数を使えば任意の周波数にそろえることができます。

```c
MotorDriver motor;
MotorDriver_Init(&motor, &htim13, TIM_CHANNEL_1, &htim14, TIM_CHANNEL_1);

// 例: 980 Hz に変更
MotorDriver_setPwmFrequency(&motor, 980U);
```

---

## `motor_driver.h` - 関数リスト

### MotorDriver_Init

**説明**  
指定されたタイマーおよびチャンネルを用いてモータードライバを初期化します。デフォルト設定では、初期化時に PWM 周波数を `g_motor_driver_default_pwm_hz`（未定義の場合は `MOTOR_DRIVER_DEFAULT_PWM_HZ`。既定: 約 980 Hz）に自動調整します。

**宣言**
```c
void MotorDriver_Init(MotorDriver *motor,
                      TIM_HandleTypeDef *htimA, uint32_t channelA,
                      TIM_HandleTypeDef *htimB, uint32_t channelB);
```

**パラメータ**
- `motor`: 初期化するモーターの構造体ポインタ。
- `htimA`: 正転用 PWM 出力のタイマー構造体。
- `channelA`: 正転用 PWM 出力のタイマーチャンネル。
- `htimB`: 逆転用 PWM 出力のタイマー構造体。
- `channelB`: 逆転用 PWM 出力のタイマーチャンネル。

**使用例（デフォルト 980 Hz を使う）**
```c
MotorDriver motor;
MotorDriver_Init(&motor, &htim13, TIM_CHANNEL_1, &htim14, TIM_CHANNEL_1);
```

**使用例（自動設定を無効化して自分で設定）**
```c
#define MOTOR_DRIVER_DISABLE_DEFAULT_PWM
#include "Altair_library_for_CubeIDE/altair.h"

MotorDriver motor;
MotorDriver_Init(&motor, &htim13, TIM_CHANNEL_1, &htim14, TIM_CHANNEL_1);
MotorDriver_setPwmFrequency(&motor, 1000U); // 1kHz に設定
```

### MotorDriver_setSpeed

**説明**  
指定された速度でモーターを回転させます。速度はパーセント指定で、負の値は逆回転を示します。

**宣言**

```c
void MotorDriver_setSpeed(MotorDriver *motor, int speed);
```

**パラメータ**

- `motor`: 設定するモーターの構造体ポインタ。
- `speed`: -100〜100 の範囲で指定。正の値で正転、負の値で逆転、0 で停止。

**使用例**

```c
MotorDriver_setSpeed(&motor, 50);   // 50% 速度で正転
MotorDriver_setSpeed(&motor, -50);  // 50% 速度で逆転
MotorDriver_setSpeed(&motor, 0);    // 停止
```

### MotorDriver_setPwmFrequency

**説明**  
`MotorDriver` が使用している 2 つのタイマの PSC/ARR を再計算し、指定した PWM 周波数に近づくように自動設定します。APB1/APB2 の分周設定を考慮し、`HAL_RCC_GetPCLK1Freq` / `HAL_RCC_GetPCLK2Freq` を用いてタイマクロックを取得します。

**宣言**

```c
HAL_StatusTypeDef MotorDriver_setPwmFrequency(MotorDriver *motor, uint32_t frequency_hz);
```

**パラメータ**

- `motor`: 対象となるモーターの構造体ポインタ。
- `frequency_hz`: 設定したい PWM 周波数 [Hz]。

**戻り値**

- `HAL_OK` : 正常に設定できた場合
- それ以外 : パラメータ不正やクロック取得失敗など

---

## `motor_driver.c` - 実装の概要

### 構造体

```c
typedef struct {
    TIM_HandleTypeDef *htimA;  // 正転側タイマ
    uint32_t          channelA;
    TIM_HandleTypeDef *htimB;  // 逆転側タイマ
    uint32_t          channelB;
} MotorDriver;
```

### MotorDriver_Init

- タイマ / チャンネルのポインタを `MotorDriver` 構造体に保存します。
- PWM を開始します（`HAL_TIM_PWM_Start`）。
- `MOTOR_DRIVER_DISABLE_DEFAULT_PWM` が定義されていなければ、`MotorDriver_setPwmFrequency` を使って PWM 周波数を `MOTOR_DRIVER_DEFAULT_PWM_HZ` に自動調整します。

### MotorDriver_setSpeed

- `speed` が正のとき: `htimA` / `channelA` に PWM を出力し、`htimB` を 0 にします。
- `speed` が負のとき: `htimB` / `channelB` に PWM を出力し、`htimA` を 0 にします。
- `speed` が 0 のとき: 両方のチャンネルを 0 にして停止します。

### MotorDriver_setPwmFrequency

- 内部関数でタイマクロックを取得し、16bit ARR に収まるよう PSC を選びます。
- 計算した PSC/ARR をレジスタに書き込み、タイマを再スタートして適用します。

---

## 使用例

### 1. デフォルト（約 920 Hz）でモーターを正転・逆転

(以下のまま貼り付けても動きません)

```c
#include "Altair_library_for_CubeIDE/altair.h"

MotorDriver motor;

void app_init(void)
{
    MX_TIM13_Init();
    MX_TIM14_Init();

    MotorDriver_Init(&motor, &htim13, TIM_CHANNEL_1, &htim14, TIM_CHANNEL_1);
}

void app_loop(void)
{
    MotorDriver_setSpeed(&motor, 50);   // 50% で正転
    HAL_Delay(2000);

    MotorDriver_setSpeed(&motor, 0);    // 停止
    HAL_Delay(1000);

    MotorDriver_setSpeed(&motor, -50);  // 50% で逆転
    HAL_Delay(2000);

    MotorDriver_setSpeed(&motor, 0);    // 停止
    HAL_Delay(1000);
}
```

### 2. 自分で PWM 周波数を決めたい場合

```c
#define MOTOR_DRIVER_DISABLE_DEFAULT_PWM
#include "Altair_library_for_CubeIDE/altair.h"

MotorDriver motor;

void app_init(void)
{
    MX_TIM13_Init();
    MX_TIM14_Init();

    MotorDriver_Init(&motor, &htim13, TIM_CHANNEL_1, &htim14, TIM_CHANNEL_1);

    // ここで 980 Hz に設定
    MotorDriver_setPwmFrequency(&motor, 980U);
}
```

### トラブルシューティング

- **モーターが動作しない場合**
  - `htimA` / `htimB` が PWM 出力に対応したタイマ・チャンネルになっているか確認してください。
  - CubeMX 側で PWM モード・ピン設定・クロック有効化が正しく行われているか確認してください。
  - `MOTOR_DRIVER_DISABLE_DEFAULT_PWM` を定義している場合、PSC/ARR が適切に設定されているか確認してください（`MotorDriver_setPwmFrequency` などを利用）。


---

[ALTAIR_MDD_V3](https://github.com/Altairu/ALTAIR_MDD_V3)


> encoder

 * A0 A1 TIMER5
  
 * B3 A15 TIMER2
  
 * B6 B7 TIMER4
  
 * C6 C7 TIMER3

> MD

 * B14(TIMER12 CH1) B15(TIMER12 CH2)
  
 * A8(TIMER1  CH1)     A9(TIMER1  CH2)
  
 * A6(TIMER13 CH1)     A7(TIMER14 CH1)
  
 * B8(TIMER10 CH1)     B9(TIMER11 CH1)

> limit switch

 * C0
  
 * C1
  
 * C2
  
 * C3

> Serial

* tx:A2  rx:A3  SERIAL2 

* tx:B10  rx:C5  SERIAL3

> CAN

* tx:A12  rx:A11  CAN1

* tx:B13  rx:B12  CAN2
