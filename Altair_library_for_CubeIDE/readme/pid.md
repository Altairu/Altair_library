# PID Library Documentation

このドキュメントでは、PID制御用ライブラリである`pid.h`と`pid.c`の使用方法について説明します。PID制御は、目標値と現在値の差（誤差）を基に、システムを目標値に近づけるように調整するための制御アルゴリズムです。このライブラリを使用することで、モーターなどの速度や位置を効率的に制御できます。

## 1. ライブラリファイル

- **pid.h**: PID制御ライブラリの関数プロトタイプ、データ構造、および設定が含まれているヘッダファイル。
- **pid.c**: PID制御ライブラリの関数の実装が含まれているソースファイル。

## 2. データ構造

### Pid

PID制御のパラメータや状態を保持する構造体です。この構造体には、制御のために必要なゲインや誤差、積分値、微分値に加えて、積分成分の上限（アンチワインドアップ用）や出力の反転設定が含まれます。

```c
typedef struct {
    double integral_error;    // 積分誤差の累積値
    double before_error;      // 前回の誤差
    double kp;                // 比例ゲイン
    double ki;                // 積分ゲイン
    double kd;                // 微分ゲイン
    double d_error;           // 微分誤差
    double p_control;         // P制御成分
    double i_control;         // I制御成分
    double d_control;         // D制御成分
    double time_constant;     // 微分フィルタの時定数（0でフィルタなし）
    double integral_limit;    // 積分誤差の絶対値上限（0で無制限）
    int    output_invert;     // 出力の符号（1: 正常, -1: 反転）
} Pid;
```

## 3. 関数の説明

### Pid_Init

PID制御構造体を初期化する関数です。この関数では、積分誤差やゲインなどの初期値を設定します。

- **プロトタイプ**:
  ```c
  void Pid_Init(Pid* pid);
  ```

- **引数**:
  - `pid`: 初期化対象のPID制御構造体のポインタ

- **説明**:
  `Pid_Init`は、積分誤差や前回誤差などの内部変数をゼロクリアし、PID制御の準備を行います。`output_invert`は既定で`1`（反転なし）に設定されます。

### Pid_setGain

PID制御に使用するゲイン（比例、積分、微分）を設定します。また、必要に応じて微分制御のフィルタ時定数を設定します。

- **プロトタイプ**:
  ```c
  void Pid_setGain(Pid* pid, double p_gain, double i_gain, double d_gain, double time_constant);
  ```

- **引数**:
  - `pid`: ゲインを設定するPID制御構造体のポインタ
  - `p_gain`: 比例ゲイン（P）
  - `i_gain`: 積分ゲイン（I）
  - `d_gain`: 微分ゲイン（D）
  - `time_constant`: 微分フィルタの時定数（0でフィルタなし）

- **説明**:
  PID制御で使用するゲインとフィルタの時定数を設定します。時定数を設定すると、微分成分にフィルタが適用され、制御の安定性を高めることができます。積分成分に上限を設けたい場合は、代わりに`Pid_setGainWithLimit`を使用してください。

### Pid_setGainWithLimit

PID制御に使用するゲイン（比例、積分、微分）と、積分誤差の絶対値上限（アンチワインドアップ用）を設定します。

- **プロトタイプ**:
  ```c
  void Pid_setGainWithLimit(Pid* pid,
                            double p_gain,
                            double i_gain,
                            double d_gain,
                            double time_constant,
                            double integral_limit);
  ```

- **引数**:
  - `pid`: ゲインを設定するPID制御構造体のポインタ
  - `p_gain`: 比例ゲイン（P）
  - `i_gain`: 積分ゲイン（I）
  - `d_gain`: 微分ゲイン（D）
  - `time_constant`: 微分フィルタの時定数（0でフィルタなし）
  - `integral_limit`: 積分誤差の絶対値上限（0以下を指定した場合は無制限）

- **説明**:
  PID制御で使用するゲインとフィルタの時定数に加え、積分誤差の絶対値上限を設定します。内部では、`Pid_control`／`Pid_controlError`の中で、`integral_error`に対してこの上限でクリップ（±`integral_limit`）が行われます。

### Pid_setInvert

PIDの出力の符号（正転／反転）を設定します。

- **プロトタイプ**:
  ```c
  void Pid_setInvert(Pid* pid, int invert);
  ```

- **引数**:
  - `pid`: 対象のPID制御構造体のポインタ
  - `invert`: 出力の符号。`1`で通常、`-1`で反転

- **説明**:
  センサやモータの配線方向の違いなどで、制御が正帰還になってしまう場合に、PIDの出力符号だけを簡単に反転させるために使用します。既定値は`1`（反転なし）です。

### Pid_control

PID制御を実行する関数です。目標値と現在値を引数に取り、その差（誤差）に基づいて制御量を計算して返します。

- **プロトタイプ**:
  ```c
  double Pid_control(Pid* pid, double target, double now, int control_period);
  ```

- **引数**:
  - `pid`: PID制御構造体のポインタ
  - `target`: 目標値
  - `now`: 現在の値
  - `control_period`: 制御周期（ms）

- **戻り値**:
  - 計算された制御量（次の操作量として設定すべき値）

- **説明**:
  目標値と現在値の差から制御量を計算し、目標値に近づけるようにします。制御周期が大きく変動する場合は、`control_period`の値を調整することで積分・微分成分に反映させます。`Pid_setGainWithLimit`で積分上限を指定している場合は、内部で積分誤差が±`integral_limit`にクリップされます。また、`Pid_setInvert`で出力を反転している場合は、最終出力に対して符号反転が適用されます。

### Pid_controlError

直接誤差を指定して制御を実行する関数です。この関数は、誤差が既に計算されている場合に使用します。

- **プロトタイプ**:
  ```c
  double Pid_controlError(Pid* pid, double error, int control_period);
  ```

- **引数**:
  - `pid`: PID制御構造体のポインタ
  - `error`: 目標値と現在値の差（誤差）
  - `control_period`: 制御周期（ms）

- **戻り値**:
  - 計算された制御量

- **説明**:
  与えられた誤差と制御周期を基に、制御量を計算します。`Pid_setGainWithLimit`や`Pid_setInvert`で設定した内容は、この関数にも反映されます。

### Pid_reset

積分誤差や微分誤差などをリセットする関数です。制御の開始やリセットを行いたいときに使用します。

- **プロトタイプ**:
  ```c
  void Pid_reset(Pid* pid);
  ```

- **引数**:
  - `pid`: リセット対象のPID制御構造体のポインタ

- **説明**:
  積分誤差や微分誤差の累積値をゼロに戻し、制御を再開します。

### Pid_getControlValue

P制御、I制御、D制御それぞれの成分を取得する関数です。

- **プロトタイプ**:
  ```c
  double Pid_getControlValue(Pid* pid, ControlType control_type);
  ```

- **引数**:
  - `pid`: PID制御構造体のポインタ
  - `control_type`: 取得する制御成分の種類（P, I, Dのいずれか）

- **戻り値**:
  - 指定された制御成分の値（`P`、`I`、または`D`）

- **説明**:
  制御成分（P、I、Dのいずれか）を取得し、デバッグやチューニングに利用できます。

## 4. 使用例

以下は、PID制御を使用してモーター角度の制御を行う簡単な例です。積分の暴走を防ぐために`Pid_setGainWithLimit`を使用し、必要に応じて`Pid_setInvert`で出力の符号を反転できます。

```c
#include "pid.h"
#include "encoder.h"
#include "motor_driver.h"
#include <stdio.h>

TIM_HandleTypeDef htim3;
TIM_HandleTypeDef htim5;
MotorDriver motor;
Encoder encoder;
EncoderData encoder_data;
Pid pid;

void main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_TIM3_Init();
    MX_TIM5_Init();

    MotorDriver_Init(&motor, &htim3, TIM_CHANNEL_1, TIM_CHANNEL_2);
    Encoder_Init(&encoder, &htim5, 100.0, 8192, 10);

    Pid_Init(&pid);
    // 比例・積分・微分ゲイン、時定数、積分誤差の上限を設定（例: ±100）
    Pid_setGainWithLimit(&pid, 1.0, 0.1, 0.01, 0.0, 100.0);
    // 必要に応じて出力の符号を反転（1: 正常, -1: 反転）
    Pid_setInvert(&pid, 1);

    double target_angle = 90.0;  // 目標角度 (度)

    while (1) {
        Encoder_Interrupt(&encoder, &encoder_data);
        double current_angle = encoder_data.deg;

        double control_value = Pid_control(&pid, target_angle, current_angle, 10);

        MotorDriver_setSpeed(&motor, (int)control_value);

        // デバッグ出力
        printf("Target Angle: %.2f, Current Angle: %.2f, Control Value: %.2f\n",
               target_angle, current_angle, control_value);

        HAL_Delay(10);  // 制御周期 10ms
    }
}
```
