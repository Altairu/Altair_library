# incenc ライブラリ（推奨）

エンコーダー用ライブラリ　

### 特徴

- **カウント取得**: エンコーダーのカウント値を取得できます。
- **角度計算**: エンコーダーの回転角度（degrees）を計算します。
- **RPS（回転数毎秒）計算**: エンコーダーの回転数を秒単位で取得できます。
- **エンコーダーデータのリセット**: 必要に応じてエンコーダーのカウントをリセットできます。

---

### 対応しているタイマーとピンセット

| タイマー | ピン A   | ピン B   |
|:---------|:---------|:---------|
| TIM5     | PA_0     | PA_1     |
| TIM2     | PB_3     | PA_5     |
| TIM3     | PC_6     | PC_7     |
| TIM4     | PB_6     | PB_7     |

---

### インストール方法

1. このリポジトリをクローンまたはダウンロードして、Mbedプロジェクトに追加してください。
2. `incenc.h`と`incenc.cpp`をプロジェクトに追加します。

---

### 使い方

#### 1. 初期化

エンコーダーを使用するためには、まず初期化を行います。`init()`関数を使用して、指定したタイマーとピンに対してエンコーダーをセットアップします。

```cpp
#include "mbed.h"
#include "incenc.h"

IncEnc encoder;
IncEncData data;

int main() {
   
    encoder.init(init(PB_3, PA_5, TIM2);

    while (true) {
        // エンコーダーデータの更新
        encoder.interrupt(TIM2, &data);

        // 角度とRPSの取得
        double angle = encoder.get_degrees(&data);
        double rps = encoder.get_rps(&data);

        // 結果を表示
        printf("角度: %.2f 度, RPS: %.2f\n", angle, rps);

        ThisThread::sleep_for(1000ms);  // 1秒ごとに表示
    }
}
```

使用例
```cpp
    encoder1.init(PA_0, PA_1, TIM5);  // TIM5 (PA_0, PA_1) 使用不可
    encoder2.init(PB_3, PA_5, TIM2);  // TIM2 (PB_3, PA_5)
    encoder3.init(PC_6, PC_7, TIM3);  // TIM3 (PC_6, PC_7)
    encoder4.init(PB_6, PB_7, TIM4);  // TIM4 (PB_6, PB_7)
```


#### 2. 関数説明

- **`init(PinName a_pin, PinName b_pin, TIM_TypeDef *tim, double diameter = 100, int ppr = 8192, int period = 1)`**
  - エンコーダーの初期化を行います。
  - `a_pin`: エンコーダーA相のピン。
  - `b_pin`: エンコーダーB相のピン。
  - `tim`: 使用するタイマー (`TIM2`, `TIM3`, `TIM4`, `TIM5`のいずれか)。
  - `diameter`: エンコーダーのホイール径 (mm)。デフォルトは100。
  - `ppr`: パルス数（1回転あたりのパルス数）。デフォルトは8192。
  - `period`: サンプリング期間（ms単位）。デフォルトは1ms。

- **`read(TIM_TypeDef *tim)`**
  - エンコーダーのカウント値を取得します。
  - `tim`: 読み取るタイマー。

- **`interrupt(TIM_TypeDef *tim, IncEncData *encoder_data)`**
  - エンコーダーデータを更新します。回転数や角度の計算も行います。

- **`reset(TIM_TypeDef *tim)`**
  - エンコーダーのカウントをリセットします。

- **`get_degrees(IncEncData *encoder_data)`**
  - 回転角度（degrees）を取得します。

- **`get_rps(IncEncData *encoder_data)`**
  - RPS（回転数毎秒）を取得します。

---

### サンプルコード

以下は、複数のエンコーダーを同時に読み取るサンプルコードです。

```cpp
#include "mbed.h"
#include "incenc.h"

IncEnc encoder1, encoder2, encoder3, encoder4;
IncEncData data1, data2, data3, data4;

int main() {
    // エンコーダーの初期化
    encoder1.init(PA_0, PA_1, TIM5);  // TIM5 (PA_0, PA_1)
    encoder2.init(PB_3, PA_5, TIM2);  // TIM2 (PB_3, PA_5)
    encoder3.init(PC_6, PC_7, TIM3);  // TIM3 (PC_6, PC_7)
    encoder4.init(PB_6, PB_7, TIM4);  // TIM4 (PB_6, PB_7)

    while (true) {
        // 各エンコーダーのデータを更新
        encoder1.interrupt(TIM5, &data1);
        encoder2.interrupt(TIM2, &data2);
        encoder3.interrupt(TIM3, &data3);
        encoder4.interrupt(TIM4, &data4);

        // 各エンコーダーの結果を表示
        printf("TIM5: 角度 = %.2f 度, RPS = %.2f\n", encoder1.get_degrees(&data1), encoder1.get_rps(&data1));
        printf("TIM2: 角度 = %.2f 度, RPS = %.2f\n", encoder2.get_degrees(&data2), encoder2.get_rps(&data2));
        printf("TIM3: 角度 = %.2f 度, RPS = %.2f\n", encoder3.get_degrees(&data3), encoder3.get_rps(&data3));
        printf("TIM4: 角度 = %.2f 度, RPS = %.2f\n", encoder4.get_degrees(&data4), encoder4.get_rps(&data4));

        ThisThread::sleep_for(1000ms);  // 1秒ごとに表示
    }
}
```

