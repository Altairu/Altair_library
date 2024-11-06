# エンコーダライブラリ

このドキュメントでは、STM32のエンコーダ用ライブラリ`encoder.h`と`encoder.c`を使用して、エンコーダの初期化、読み取り、リセット、割り込み処理を行う方法について説明します。このライブラリは、エンコーダのA相とB相の信号をSTM32のタイマーで読み取り、回転数や角度、距離、速度などのデータを計算します。

## 1. 概要

エンコーダは、モーターなどの回転物の位置や速度、回転数を測定するために使用されます。このライブラリでは、STM32のタイマーをエンコーダモードで動作させ、エンコーダからの2相信号（A相とB相）を利用してカウントデータを取得し、計算処理を行います。

## 2. ライブラリファイル

- **encoder.h**: エンコーダライブラリの関数プロトタイプ、データ構造、設定が含まれているヘッダファイルです。
- **encoder.c**: エンコーダライブラリの関数の実装が含まれているソースファイルです。

## 3. データ構造

### EncoderData

エンコーダの計測データを格納する構造体です。

```c
typedef struct {
    int count;         // エンコーダのカウント数
    double rot;        // 回転数（回転の合計数、単位は回）
    double deg;        // 回転角度（度数）
    double distance;   // 移動距離（回転数とホイール直径から計算）
    double velocity;   // 移動速度（mm/s、距離と時間間隔から計算）
    double rps;        // 毎秒の回転数（回転数/秒）
} EncoderData;
```

### Encoder

エンコーダの設定や状態を保持する構造体です。

```c
typedef struct {
    TIM_HandleTypeDef* htim;  // タイマーのハンドル
    int ppr;                  // エンコーダのパルス数（1回転あたりのパルス数）
    double diameter;          // エンコーダに接続されるホイールの直径（mm）
    int period;               // 読み取り周期（ms）
    int limit;                // カウントオーバーフローのリミット
    double before_rot;        // 前回の回転数（速度計算用）
} Encoder;
```

## 4. 関数の説明

### Encoder_Init

エンコーダを初期化する関数です。この関数は、指定されたタイマーをエンコーダモードに設定し、エンコーダのパラメータを初期化します。

- **プロトタイプ**:
  ```c
  void Encoder_Init(Encoder* encoder, TIM_HandleTypeDef* htim, double diameter, int ppr, int period);
  ```

- **引数**:
  - `encoder`: エンコーダの設定と状態を格納する構造体のポインタ
  - `htim`: エンコーダ用のタイマー（例: TIM5）のハンドル
  - `diameter`: ホイールの直径（mm）
  - `ppr`: 1回転あたりのパルス数（エンコーダの分解能）
  - `period`: 読み取り周期（ms）

- **説明**:
  タイマーをエンコーダモードで動作させ、A相とB相の信号をカウントします。また、エンコーダのパラメータ（直径やPPR）を設定します。

### Encoder_Read

エンコーダのカウント値を読み取る関数です。

- **プロトタイプ**:
  ```c
  int Encoder_Read(Encoder* encoder);
  ```

- **引数**:
  - `encoder`: エンコーダの構造体のポインタ

- **戻り値**:
  - エンコーダのカウント値（現在のカウンタ値）

- **説明**:
  タイマーの現在のカウント値を取得し、エンコーダの位置情報として利用します。

### Encoder_Interrupt

エンコーダのデータを計算・更新する関数です。定期的に呼び出して、回転数や角度、移動距離、速度などを計算します。

- **プロトタイプ**:
  ```c
  void Encoder_Interrupt(Encoder* encoder, EncoderData* encoder_data);
  ```

- **引数**:
  - `encoder`: エンコーダの設定と状態を格納する構造体のポインタ
  - `encoder_data`: エンコーダの計測データを格納する構造体のポインタ

- **説明**:
  `Encoder_Read`で取得したカウント値から、以下のデータを計算して`encoder_data`に格納します。
  - `rot`: 総回転数（回）
  - `deg`: 回転角度（度数）
  - `distance`: 移動距離（mm）
  - `velocity`: 移動速度（mm/s）
  - `rps`: 毎秒の回転数（回転数/秒）

### Encoder_Reset

エンコーダのカウンタ値をリセットする関数です。

- **プロトタイプ**:
  ```c
  void Encoder_Reset(Encoder* encoder);
  ```

- **引数**:
  - `encoder`: エンコーダの構造体のポインタ

- **説明**:
  タイマーのカウンタをリセットして、エンコーダのカウントを0に戻します。オーバーフロー対策として利用することができます。

## 5. 使用例

以下に、エンコーダの初期化とデータの読み取りの例を示します。

```c
#include "encoder.h"
#include "stm32f4xx_hal.h"

// タイマーとエンコーダの構造体を定義
TIM_HandleTypeDef htim5;
Encoder encoder;
EncoderData encoder_data;

void main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_TIM5_Init();  // TIM5の初期化関数

    // エンコーダの初期化
    Encoder_Init(&encoder, &htim5, 100.0, 8192, 10);  // 直径100mm, パルス数8192, 読み取り周期10ms

    while (1) {
        // エンコーダのデータを更新
        Encoder_Interrupt(&encoder, &encoder_data);

        // データを使用
        printf("Count: %d, Rotation: %.2f, Degree: %.2f, Distance: %.2f, Velocity: %.2f\n",
               encoder_data.count, encoder_data.rot, encoder_data.deg,
               encoder_data.distance, encoder_data.velocity);

        HAL_Delay(10);  // 10msごとにデータ取得
    }
}
```

## 6. 注意事項

- **カウンタのオーバーフロー**: タイマーのカウンタはオーバーフローすることがあります。高精度なエンコーダを使用する場合、カウンタオーバーフローに注意し、`Encoder_Reset`関数で適宜リセットするか、カウント範囲の確認が必要です。
- **浮動小数点数のサポート**: 浮動小数点数（`%f`）を使って`printf`で表示する場合、STM32CubeIDEで浮動小数点サポートを有効にする設定（`-u _printf_float`）が必要です。

---

```
The float formatting support is not enabled, check your MCU Settings from "Project Properties > C/C++ Build > Settings > Tool Settings", or add manually "-u _printf_float" in linker flags.
```
`printf`や`snprintf`で浮動小数点数（`%f`などのフォーマット指定子）を使う場合に発生することがあります。STM32の標準設定では、浮動小数点のサポートが無効化されているため、明示的に有効にする必要があります。

以下の手順で、浮動小数点のサポートを有効にしましょう。

### 手順 1: プロジェクトの設定を変更

1. **プロジェクトを右クリック**し、**[Properties]**（プロパティ）を選択します。
2. 左側のメニューから**[C/C++ Build] > [Settings]**に移動します。
3. **[Tool Settings]**タブを選択し、以下のように設定します。

#### リンカオプションの設定
- **[MCU GCC Linker] > [Miscellaneous]** の項目に移動します。
- **[Linker flags]** の欄に、`-u _printf_float`を追加します。

   - 具体的には、リンクオプションのフィールドに次のように入力してください:
     ```
     -u _printf_float
     ```

### 手順 2: ビルドの再実行

この設定を追加したら、プロジェクトを再ビルドしてください。これで`printf`や`snprintf`で浮動小数点数を含むフォーマット指定子（`%f`など）が正しく使用できるようになるはずです。

---

シリアル見方
```bash
sudo apt update
sudo apt install screen
screen /dev/ttyUSB0 115200
```