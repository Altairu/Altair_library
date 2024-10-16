
## エンコーダーライブラリの使用方法

### 概要

このライブラリは、STM32 Nucleoボード上でエンコーダーの読み取りを簡単に実行できるようにするためのものです。エンコーダーのカウント、角度、RPS（回転数）、距離などのデータを取得し、リセットする機能を提供します。

### 対応するエンコーダー設定

以下のタイマーとピン設定を使用します:

- **TIM2**: PB_3 (CH1), PA_5 (CH2)
- **TIM3**: PC_6 (CH1), PC_7 (CH2)
- **TIM4**: PB_6 (CH1), PB_7 (CH2)

**注意**: TIM5 (PA_0, PA_1) は使用できません。

---

### 使用方法

1. **ライブラリの初期化**

各エンコーダーを初期化します。以下の例は、TIM2、TIM3、TIM4に接続されたエンコーダーを初期化するコードです。

```cpp
#include "mbed.h"
#include "incenc.h"

IncEnc encoder1, encoder2, encoder3;
IncEncData data1, data2, data3;

int main() {
    // 各エンコーダーの初期化
    encoder1.init(PB_3, PA_5, TIM2);  // TIM2 (PB_3, PA_5)
    encoder2.init(PC_6, PC_7, TIM3);  // TIM3 (PC_6, PC_7)
    encoder3.init(PB_6, PB_7, TIM4);  // TIM4 (PB_6, PB_7)

    while (true) {
        // エンコーダーデータの取得
        encoder1.getEncoderData(TIM2, &data1);
        encoder2.getEncoderData(TIM3, &data2);
        encoder3.getEncoderData(TIM4, &data3);

        // 結果の表示
        printf("Encoder1 (TIM2): Count = %d, RPS = %.2f, Degree = %.2f, Distance = %.2f\n",
               data1.count, data1.rps, data1.deg, data1.distance);
        printf("Encoder2 (TIM3): Count = %d, RPS = %.2f, Degree = %.2f, Distance = %.2f\n",
               data2.count, data2.rps, data2.deg, data2.distance);
        printf("Encoder3 (TIM4): Count = %d, RPS = %.2f, Degree = %.2f, Distance = %.2f\n",
               data3.count, data3.rps, data3.deg, data3.distance);

        ThisThread::sleep_for(1000ms);  // 1秒ごとにデータを表示
    }
}
```

2. **エンコーダーデータの取得**

各エンコーダーからカウントやRPS、角度、距離のデータを取得するためには、`getEncoderData()`関数を使用します。

```cpp
encoder1.getEncoderData(TIM2, &data1);
```

この関数は、以下のようなデータを`IncEncData`構造体に格納します。

- `count`: カウント値
- `rps`: 回転数 (RPS)
- `deg`: 角度 (度)
- `distance`: 回転距離

3. **エンコーダーのリセット**

エンコーダーのカウントをリセットするには、`reset()`関数を使用します。

```cpp
encoder1.reset(TIM2);
```

この関数は、指定したエンコーダーのカウントを0にリセットします。

---
取得方法を具体的に説明します。エンコーダーデータの各値（カウント、RPS、角度、回転距離）の取得手順について説明します。

### 各データの取得方法

エンコーダーデータを取得するには、次の手順で`getEncoderData()`を使用します。

1. **カウント値 (`count`)**
   - エンコーダーの現在のカウント値は、`getEncoderData()`で取得されます。
   - `IncEncData`構造体の`count`メンバにカウント値が格納されます。

   ```cpp
   encoder.getEncoderData(TIM2, &data1);
   printf("カウント値 = %d\n", data1.count);
   ```

2. **回転数 (RPS)**
   - `getEncoderData()`を呼び出すと、`IncEncData`構造体の`rps`メンバにRPS（1秒あたりの回転数）が格納されます。

   ```cpp
   encoder.getEncoderData(TIM2, &data1);
   printf("RPS = %.2f\n", data1.rps);
   ```

3. **角度 (`deg`)**
   - エンコーダーの回転軸が何度回転したか（0°～360°）は、`IncEncData`構造体の`deg`メンバに格納されます。

   ```cpp
   encoder.getEncoderData(TIM2, &data1);
   printf("角度 (度) = %.2f\n", data1.deg);
   ```

4. **回転距離 (`distance`)**
   - 回転軸やホイールが回転によって移動した距離は、`IncEncData`構造体の`distance`メンバに格納されます。

   ```cpp
   encoder.getEncoderData(TIM2, &data1);
   printf("回転距離 = %.2fmm\n", data1.distance);
   ```

---

### コード例

以下のコードは、エンコーダーからデータを取得する具体例です。`TIM2`を使って、カウント値、RPS、角度、回転距離を1秒ごとに取得・表示します。

```cpp
#include "mbed.h"
#include "incenc.h"

IncEnc encoder1;
IncEncData data1;

int main() {
    // エンコーダーの初期化 (例: TIM2)
    encoder1.init(PB_3, PA_5, TIM2);

    while (true) {
        // エンコーダーデータの取得
        encoder1.getEncoderData(TIM2, &data1);

        // 各値を表示
        printf("カウント値 = %d\n", data1.count);
        printf("RPS = %.2f\n", data1.rps);
        printf("角度 (度) = %.2f\n", data1.deg);
        printf("回転距離 = %.2fmm\n", data1.distance);

        ThisThread::sleep_for(1000ms);  // 1秒ごとにデータを表示
    }
}
```

### リセット方法

エンコーダーのカウント値をリセットしたい場合は、`reset()`関数を使用します。これにより、エンコーダーのカウントが0にリセットされます。

```cpp
encoder1.reset(TIM2);  // TIM2エンコーダーのカウントをリセット
```


---

### 開発環境

- **プラットフォーム**: STM32 Nucleo F446RE
- **フレームワーク**: Mbed OS
- **コンパイラ**: PlatformIO

### ビルド手順

1. `platformio.ini` ファイルに以下の設定を追加して、Mbed OSを使用します。

```ini
[env:nucleo_f446re]
platform = ststm32
board = nucleo_f446re
framework = mbed
build_flags = 
    -DMBED_CONF_TARGET_DEFAULT_TICKER_TIMER=9
    -DMBED_TICKLESS
```

2. コードをビルドし、Nucleoボードに書き込みます。

```bash
pio run --target upload
```

3. シリアルモニタを使用して、出力結果を確認します。

---

### 注意事項

- **TIM5 (PA_0, PA_1) は使用できません**。そのため、他のタイマーでエンコーダーを設定してください。
- 各エンコーダーは専用のタイマーを使用する必要があります。同じタイマーで複数のエンコーダーを読み取ることはできません。
