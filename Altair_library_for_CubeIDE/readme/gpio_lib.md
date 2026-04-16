# gpio_lib.md

## GPIO/PWMライブラリ: `gpio_lib`

`gpio_lib` は、STM32 HALのGPIO操作を簡単に扱うための薄いラッパです。
通常の入出力に加えて、CubeMXで生成したタイマ設定を使ったPWM Duty制御に対応します。

---

## 1. 概要

- GPIO出力/入力/プルアップ/プルダウン初期化
- GPIO書き込み、トグル、読み取り
- PWM開始とDuty比(%)設定

---

## 2. 必要ファイル

- `gpio_lib.h`
- `gpio_lib.c`

---

## 3. API

### 3.1 初期化（GPIO）

```c
HAL_StatusTypeDef GpioLib_Init(GpioLib *gpio, GPIO_TypeDef *port, uint16_t pin, GpioLibMode mode);
```

- `mode`:
  - `GPIO_LIB_MODE_OUTPUT`
  - `GPIO_LIB_MODE_INPUT`
  - `GPIO_LIB_MODE_INPUT_PULLUP`
  - `GPIO_LIB_MODE_INPUT_PULLDOWN`

### 3.2 初期化（PWM）

```c
HAL_StatusTypeDef GpioLib_InitPwm(GpioLib *gpio, GPIO_TypeDef *port, uint16_t pin,
                                  TIM_HandleTypeDef *htim, uint32_t channel);
```

- CubeMXで対象タイマ・チャネルをPWMに設定しておく必要があります。
- この関数内で `HAL_TIM_PWM_Start` を実行します。

### 3.3 出力/入力

```c
void GpioLib_Write(GpioLib *gpio, GPIO_PinState state);
void GpioLib_Toggle(GpioLib *gpio);
GPIO_PinState GpioLib_Read(GpioLib *gpio);
```

### 3.4 Duty設定

```c
void GpioLib_WriteDuty(GpioLib *gpio, float duty_percent);
```

- `duty_percent` は 0.0〜100.0 を想定
- 範囲外入力は内部でクリップされます

---

## 4. 使用例

### 4.1 GPIO出力（LED点滅）

```c
#include "Altair_library_for_CubeIDE/gpio_lib.h"

extern void SystemClock_Config(void);
extern void MX_GPIO_Init(void);

int main(void)
{
    GpioLib led;

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();

    GpioLib_Init(&led, GPIOC, GPIO_PIN_13, GPIO_LIB_MODE_OUTPUT);

    while (1) {
        GpioLib_Toggle(&led);
        HAL_Delay(500);
    }
}
```

### 4.2 PWM Duty変更

```c
#include "Altair_library_for_CubeIDE/gpio_lib.h"

extern TIM_HandleTypeDef htim1;
extern void SystemClock_Config(void);
extern void MX_GPIO_Init(void);
extern void MX_TIM1_Init(void);

int main(void)
{
    GpioLib pwm;

    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_TIM1_Init();

    GpioLib_InitPwm(&pwm, GPIOA, GPIO_PIN_8, &htim1, TIM_CHANNEL_1);

    while (1) {
        GpioLib_WriteDuty(&pwm, 25.0f);
        HAL_Delay(500);
        GpioLib_WriteDuty(&pwm, 75.0f);
        HAL_Delay(500);
    }
}
```

---

## 5. 注意点

- GPIOクロック有効化は通常CubeMX生成コード側で行われます。
- PWMピンのAlternate Function設定もCubeMX側で正しく設定してください。
- `GpioLib_InitPwm` ではポート/ピンを直接再初期化しないため、ピン設定はCubeMX準拠で使う前提です。
