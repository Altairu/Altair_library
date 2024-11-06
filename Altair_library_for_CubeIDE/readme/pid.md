# PID Library Documentation

このドキュメントでは、PID制御用ライブラリである`pid.h`と`pid.c`の使用方法について説明します。PID制御は、目標値と現在値の差（誤差）を基に、システムを目標値に近づけるように調整するための制御アルゴリズムです。このライブラリを使用することで、モーターなどの速度や位置を効率的に制御できます。

## 1. ライブラリファイル

- **pid.h**: PID制御ライブラリの関数プロトタイプ、データ構造、および設定が含まれているヘッダファイル。
- **pid.c**: PID制御ライブラリの関数の実装が含まれているソースファイル。

## 2. データ構造

### Pid

PID制御のパラメータや状態を保持する構造体です。この構造体には、制御のために必要なゲインや誤差、積分値、微分値などが含まれます。

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
  `Pid_Init`は、積分誤差や前回誤差などの内部変数をゼロクリアし、PID制御の準備を行います。

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
  PID制御で使用するゲインとフィルタの時定数を設定します。時定数を設定すると、微分成分にフィルタが適用され、制御の安定性を高めることができます。

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
  目標値と現在値の差から制御量を計算し、目標値に近づけるようにします。制御周期が大きく変動する場合は、`control_period`の値を調整することで積分・微分成分に反映させます。

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
  与えられた誤差と制御周期を基に、制御量を計算します。

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

以下は、PID制御を使用してモーター速度や角度の制御を行う例です。

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
    Pid_setGain(&pid, 1.0, 0.1, 0.01, 0.0);  // PIDゲイン設定
    
    double target_angle = 90.0;  // 目標角度 (度)

    while (1) {
        Encoder_Interrupt(&encoder, &encoder_data);
        double current_angle = encoder_data.deg;

        double control_value = Pid_control(&pid, target_angle, current_angle, 10);

        MotorDriver_setSpeed(&motor, control_value);

        // デバッグ出力
        printf("Target Angle: %.2f, Current Angle: %.2f, Control Value: %.2f\n",
               target_angle, current_angle, control_value);

        HAL_Delay(10);  // 制御周期 10ms
    }
}
```
