# Serial.md

## シリアル通信ライブラリ: `serial_lib`

このドキュメントでは、STM32マイコンにおけるシリアル通信を行うためのライブラリ`serial_lib`の使い方や関数の解説を行います。このライブラリを用いると、ヘッダ付きで複数のデータを簡単に送受信することができます。


## 1. 概要

`serial_lib`ライブラリは、UART経由でのデータ通信を容易に行うためのライブラリです。以下の機能を提供しています。
- 2バイトのヘッダ付きデータパケットの送信
- ヘッダ付きデータパケットの受信とデータの整列
- データ数が可変のため、柔軟なデータパケットを作成可能

---

## 2. ライブラリの使用方法

### 2.1 必要なファイル

`serial_lib.h`と`serial_lib.c`をプロジェクトに追加してください。

### 2.2 `serial_lib.h`のインクルード

メインプログラムで以下のように`serial_lib.h`をインクルードします。

```c
#include "serial_lib.h"
```

### 2.3 初期化

シリアル通信を初期化するには、UARTハンドルを`Serial_Init`に渡します。

```c
UART_HandleTypeDef huart2;
Serial_Init(&huart2);
```

---

## 3. ライブラリ関数の説明

### 3.1 `Serial_Init`

```c
void Serial_Init(UART_HandleTypeDef *huart);
```

**説明**: UARTを使ってシリアル通信を行うための初期設定を行います。

**パラメータ**
- `huart`: UARTのハンドルポインタ

---

### 3.2 `Serial_SendData`

```c
void Serial_SendData(UART_HandleTypeDef *huart, int16_t *data, uint8_t data_count);
```

**説明**: データ数に応じて指定された`data`を、ヘッダ付きでUARTから送信します。

**パラメータ**
- `huart`: UARTのハンドルポインタ
- `data`: 送信したいデータ配列
- `data_count`: 送信するデータの数（データの要素数）

**使用例**

```c
int16_t data_to_send[3] = {100, 200, -150};
Serial_SendData(&huart2, data_to_send, 3);
```

この例では、100、200、-150という3つのデータを送信します。

---

### 3.3 `Serial_ReceiveData`

```c
uint8_t Serial_ReceiveData(UART_HandleTypeDef *huart, int16_t *data, uint8_t data_count);
```

**説明**: UARTからデータを受信します。ヘッダを確認し、正しい形式であればデータを`data`配列に格納します。

**パラメータ**
- `huart`: UARTのハンドルポインタ
- `data`: 受信データを格納する配列
- `data_count`: 期待するデータの数（配列要素数）

**戻り値**
- `1`: 正常にデータを受信した場合
- `0`: データが不正または受信エラーの場合

**使用例**

```c
int16_t received_data[3];
if (Serial_ReceiveData(&huart2, received_data, 3)) {
    // 受信成功、`received_data`にデータが格納されています
}
```

---

## 4. メインプログラムの例

以下のコードは、シリアル通信で3つのデータ（例：Vx, Vy, ω）を受信し、運動学を使用して各モーター速度を計算し、それを送り返す例です。

```c
#include "main.h"
#include "motor_driver.h"
#include "serial_lib.h"
#include "kinematics.h"

UART_HandleTypeDef huart2;
Kinematics kinematics;
MotorDriver motorFR, motorFL, motorBR;

int16_t Vx, Vy, omega;
float speedFR, speedFL, speedBR;

void SystemClock_Config(void);
void MX_GPIO_Init(void);
void MX_USART2_UART_Init(void);

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    MX_USART2_UART_Init();

    Serial_Init(&huart2);
    
    while (1) {
        int16_t control_data[3];

        if (Serial_ReceiveData(&huart2, control_data, 3)) {
            Vx = control_data[0];
            Vy = control_data[1];
            omega = control_data[2];

            Kinematics_GetTargetSpeeds(&kinematics, (float)Vx, (float)Vy, (float)omega, &speedFR, &speedFL, &speedBR, NULL);

            int16_t motor_speeds[3] = {(int16_t)speedFR, (int16_t)speedFL, (int16_t)speedBR};
            Serial_SendData(&huart2, motor_speeds, 3);
        }
    }
}
```

---

## 5. Python側の送受信例

以下のPythonスクリプトは、Vx, Vy, ωの3つのデータをSTM32に送信し、受信データを表示する例です。

```python
import serial
import struct
import time

# シリアルポートの設定
port = "/dev/ttyACM0"  # Linuxの場合
baudrate = 115200

try:
    ser = serial.Serial(port, baudrate, timeout=0.1)
    print("Connected to", port)

    while True:
        # 3つのデータ (Vx, Vy, ω) を送信
        Vx = 100
        Vy = 200
        omega = -50
        send_data = struct.pack('>BBhhh', 0xA5, 0xA5, Vx, Vy, omega)
        ser.write(send_data)

        # 8バイト受信: ヘッダー2バイト + データ6バイト
        if ser.in_waiting >= 8:
            received_data = ser.read(8)
            if received_data[0] == 0xA5 and received_data[1] == 0xA5:
                _, _, speedFR, speedFL, speedBR = struct.unpack('>BBhhh', received_data)
                print("Received from STM32:", speedFR, speedFL, speedBR)

        time.sleep(0.1)

except serial.SerialException as e:
    print("Serial error:", e)

finally:
    if ser.is_open:
        ser.close()
        print("Connection closed")
```

