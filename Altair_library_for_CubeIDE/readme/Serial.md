
# Serial.md

## 1. STM32でのシリアル通信の設定 (CubeMXを使用)

STM32でPCとのシリアル通信を行うには、CubeMXでUSART設定を行います。以下の手順に従って設定を行ってください。

### CubeMX設定手順

1. **CubeMXのプロジェクトを新規作成**または開く。
2. **ペリフェラルの設定**: `Connectivity` カテゴリの `USART2` (または任意のUSART)を選択して有効化します。
3. **ピン設定**: CubeMXが自動的にTXとRXピンを選択するので確認します。
4. **ボーレート設定**: `Configuration` タブでボーレートを設定します (例: `115200` bps)。
5. **クロック設定**: クロック速度を設定し、USARTが希望の速度で動作することを確認します。
6. **プロジェクトを生成**: `Generate Code` をクリックしてプロジェクトを生成します。

## 2.  `serial_lib` ライブラリの使い方

このライブラリを使うことで、STM32とPC間でシリアル通信を簡単に行うことができます。通信データに特定のヘッダー (`0xA5 0xA5`) を付加するため、データの整合性を簡単に確認できます。

### ライブラリ概要

- **ヘッダー**: データの先頭に `0xA5 0xA5` を追加し、通信データの開始を示します。
- **送信**: `Serial_SendData` を使って、任意の3つの16ビット整数データを送信します。
- **受信**: `Serial_ReceiveData` を使って、特定のヘッダー付きデータを受信します。受信成功時にデータを格納します。

### 1. 初期設定

#### `serial_lib.h` と `serial_lib.c` のプロジェクトへの追加

1. `serial_lib.h` と `serial_lib.c` ファイルをプロジェクトの適切なフォルダに追加します (例: `Core/Inc` と `Core/Src`)。
2. メインファイル (例: `main.c`) に `serial_lib.h` をインクルードします。

   ```c
   #include "serial_lib.h"
   ```

#### CubeMXでの設定

`CubeMX` で `USART2` を設定している前提です。プロジェクトで別のUSARTを使用する場合、適宜変更してください。

1. `USART2` を有効化し、ボーレートなどの設定を行います。
2. `GPIO` 設定で `USART2_TX` と `USART2_RX` ピンを確認してください。
3. `Project Manager` でプロジェクトを再生成し、初期化コードを生成します。

### 2. 関数の使用方法

#### 1. 初期化

まず、通信を行う前に初期化します。このライブラリでは、`HAL_UART_Receive_IT` 関数を使ってUSARTハンドルを初期化します。

```c
Serial_Init(&huart2);  // USART2を例に初期化
```

#### 2. データ送信

`Serial_SendData` 関数を使って、任意の3つの整数データを送信します。

```c
int16_t speedFR = 50;
int16_t speedFL = 25;
int16_t speedBR = -30;

Serial_SendData(&huart2, speedFR, speedFL, speedBR);
```

- **パラメータ**:
  - `&huart2` : 初期化された `UART_HandleTypeDef` インスタンス (USARTハンドル)。
  - `speedFR`, `speedFL`, `speedBR`: 16ビット整数の送信データ。
- この関数を呼び出すと、送信データの先頭に `0xA5 0xA5` が付加され、8バイトのデータが送信されます。

#### 3. データ受信

`Serial_ReceiveData` 関数を使ってデータを受信し、指定した変数に格納します。

```c
int16_t receivedFR, receivedFL, receivedBR;

if (Serial_ReceiveData(&huart2, &receivedFR, &receivedFL, &receivedBR)) {
    // データ受信成功時の処理
    MotorDriver_setSpeed(&motorFR, receivedFR);
    MotorDriver_setSpeed(&motorFL, receivedFL);
    MotorDriver_setSpeed(&motorBR, receivedBR);
}
```

- **パラメータ**:
  - `&huart2` : 初期化された `UART_HandleTypeDef` インスタンス。
  - `&receivedFR`, `&receivedFL`, `&receivedBR`: 受信データを格納する変数へのポインタ。
- **戻り値**: 受信が成功すると `1` が返され、失敗すると `0` が返されます。

### 3. Pythonコードでの使用例

Python側から3つのモーター速度データをSTM32に送信し、STM32から送信されたデータを受信する場合、次のコードを使用します。

```python
import serial
import struct
import time

# シリアルポートの設定
port = "/dev/ttyACM0"
baudrate = 115200

try:
    ser = serial.Serial(port, baudrate, timeout=0.1)
    print("Connected to", port)

    while True:
        # データ送信 (-50, 50, 100)
        speedFR = -50
        speedFL = 50
        speedBR = 100
        send_data = struct.pack('>BBhhh', 0xA5, 0xA5, speedFR, speedFL, speedBR)
        ser.write(send_data)

        # データ受信
        if ser.in_waiting >= 8:
            received_data = ser.read(8)
            if received_data[:2] == b'\xA5\xA5':
                _, _, fr, fl, br = struct.unpack('>BBhhh', received_data)
                print("Received from STM32:", fr, fl, br)

        time.sleep(0.1)

except serial.SerialException as e:
    print("Serial error:", e)

finally:
    if ser.is_open:
        ser.close()
        print("Connection closed")
```


## 3. 使用例

このライブラリを使って、3つのモータースピードを受信し、STM32上でモーターの速度制御に適用するサンプルコードを以下に示します。

### STM32側のサンプルコード

```c
#include "main.h"
#include "serial_lib.h"
#include "motor_driver.h"

// モータードライバとUARTのインスタンス
MotorDriver motorFR, motorFL, motorBR;
UART_HandleTypeDef huart2;

int main(void) {
    HAL_Init();
    SystemClock_Config();
    MX_USART2_UART_Init();

    Serial_Init(&huart2);

    // モータードライバ初期化
    MotorDriver_Init(&motorFR, &htim12, TIM_CHANNEL_1, &htim12, TIM_CHANNEL_2);
    MotorDriver_Init(&motorFL, &htim1, TIM_CHANNEL_1, &htim1, TIM_CHANNEL_4);
    MotorDriver_Init(&motorBR, &htim13, TIM_CHANNEL_1, &htim14, TIM_CHANNEL_1);

    while (1) {
        int16_t speedFR, speedFL, speedBR;

        if (Serial_ReceiveData(&huart2, &speedFR, &speedFL, &speedBR)) {
            MotorDriver_setSpeed(&motorFR, (int)speedFR);
            MotorDriver_setSpeed(&motorFL, (int)speedFL);
            MotorDriver_setSpeed(&motorBR, (int)speedBR);

            Serial_SendData(&huart2, speedFR, speedFL, speedBR);  // 受信データを返送
        }
    }
}
```

### Python側のサンプルコード

Pythonを使って、3つのモータースピードをSTM32に送信し、受信したデータを確認するコードを以下に示します。

```python
import serial
import struct
import time

# シリアルポートの設定
port = "/dev/ttyACM0"  # Linux用ポート
baudrate = 115200

try:
    # シリアル接続の初期化
    ser = serial.Serial(port, baudrate, timeout=0.1)
    print("Connected to", port)

    while True:
        # 3つの速度を送信 (-100, 50, 200)
        speedFR = -100
        speedFL = 50
        speedBR = 200
        send_data = struct.pack('>BBhhh', 0xA5, 0xA5, speedFR, speedFL, speedBR)
        ser.write(send_data)

        # STM32からの返信を受信して表示
        if ser.in_waiting >= 8:
            received_data = ser.read(8)
            if received_data[:2] == b'\xA5\xA5':
                _, _, fr, fl, br = struct.unpack('>BBhhh', received_data)
                print("Received from STM32:", fr, fl, br)

        time.sleep(0.1)

except serial.SerialException as e:
    print("Serial error:", e)

finally:
    if ser.is_open:
        ser.close()
        print("Connection closed")
```

