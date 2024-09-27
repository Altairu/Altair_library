# AltairSerial

AltairSerialは、Mbedプラットフォームで動作するSTM32向けの自作シリアル通信ライブラリです。このライブラリは、USB経由でシリアル通信を行い、`float`型のデータやその他のデータ型を送受信する機能を提供します。データはヘッダーとして`0xA5`を追加して送信されるため、受信側でデータの同期が簡単に行えます。

## 特徴
- 複数のUSBモードに対応（`USB_A`, `USB_B`, `USB_MiniB`）
- データ送信時に自動でヘッダー`0xA5`を付加
- `float`型データを配列として送受信可能
- ボーレートのカスタマイズが可能（デフォルトは9600bps）

## 対応ピン
- **USB_A**: `PA_9` (TX), `PA_10` (RX)
- **USB_B**: `PC_10` (TX), `PC_11` (RX)
- **USB_MiniB**: `USBTX`, `USBRX`

## 使用方法

### 初期化

```cpp
#include "AltairSerial.h"

AltairSerial serial(USB_A, 115200); // USB_Aモードで初期化、ボーレート115200bps
```

### float型データの送信

```cpp
float dataToSend[] = {3.14f, 1.618f, 2.718f};
serial.sendFloatArrayWithHeader(dataToSend, 3);  // 3つのfloat型データを送信
```

### float型データの受信

```cpp
float receivedData[10];  // 受信するデータ用のバッファ
int length = 10;         // 受信するfloat型データの長さを指定
serial.receiveFloatArrayWithHeader(receivedData, length);  // 受信開始

// 受信したデータを表示
for (int i = 0; i < length; i++) {
    printf("Received float: %f\n", receivedData[i]);
}
```

## Pythonでの通信例

Python側では、PySerialを使用してSTM32とシリアル通信を行い、同じフォーマットでデータを送受信できます。

### インストール

```bash
pip install pyserial
```

### Pythonでのデータ受信例

```python
import serial
import struct

# シリアルポートの設定（ポートは環境に合わせて変更）
ser = serial.Serial('COM3', 115200, timeout=1)

def receive_float_array():
    while True:
        # ヘッダー 0xA5 を待つ
        if ser.read(1) == b'\xA5':
            # データ長を受信
            length = ser.read(1)
            if length:
                length = struct.unpack('B', length)[0]
                # float データを受信
                data = []
                for _ in range(length):
                    float_bytes = ser.read(4)
                    if len(float_bytes) == 4:
                        value = struct.unpack('f', float_bytes)[0]
                        data.append(value)
                return data

if __name__ == "__main__":
    float_array = receive_float_array()
    print("Received float array:", float_array)
```

## APIリファレンス

### コンストラクタ

```cpp
AltairSerial(USBMode mode, int baudRate = 9600);
```
- **mode**: 使用するUSBモード（`USB_A`, `USB_B`, `USB_MiniB`）
- **baudRate**: ボーレート（デフォルトは9600bps）

### データ送信

```cpp
void sendFloatArrayWithHeader(float* data, int length);
```
- **sendFloatArrayWithHeader(float* data, int length)**: `float`型の配列を送信。ヘッダー`0xA5`を自動で付加。

### データ受信

```cpp
void receiveFloatArrayWithHeader(float* buffer, int& length);
```
- **receiveFloatArrayWithHeader(float* buffer, int& length)**: `0xA5`のヘッダーを認識し、`float`型配列を受信します。

## 注意点
- 送信と受信のデータ長は事前に設定しておく必要があります。
- `float`データのサイズは4バイトであるため、送信する配列の長さとサイズに注意してください。

## 必要な設定

`platformio.ini`に以下の設定を追加して、`float`型のサポートを有効にしてください。

```ini
build_flags = 
    -D PIO_FRAMEWORK_MBED_RTOS_PRESENT
    -D MBED_MINIMAL_PRINTF_ENABLE_FLOATING_POINT
    -D MBED_MINIMAL_PRINTF_ENABLE_64_BIT
```

### STM32F446RE マイコン側のサンプルコード (`main.cpp`)

このコードは、STM32側で3つの`float`データを送信し、PC側からのデータを受信します。

```cpp
#include "mbed.h"
#include "AltairSerial.h"

// AltairSerialをUSB_Aモードでボーレート115200bpsで初期化
AltairSerial altair(USB_A, 115200);

int main() {
    // 送信するfloatデータ
    float dataToSend[] = {1.23f, 4.56f, 7.89f};
    int dataLength = sizeof(dataToSend) / sizeof(float);

    // データ送信
    printf("Sending data to PC...\n");
    altair.sendFloatArrayWithHeader(dataToSend, dataLength);

    // 受信するバッファ
    float receivedData[3];
    int receivedLength = 3;

    // データをPCから受信
    printf("Waiting for data from PC...\n");
    altair.receiveFloatArrayWithHeader(receivedData, receivedLength);

    // 受信したデータを表示
    printf("Received data from PC:\n");
    for (int i = 0; i < receivedLength; i++) {
        printf("Received float: %f\n", receivedData[i]);
    }

    while (true) {
        // メインループ
    }
}
```

### PC側（Python）のサンプルコード

このPythonコードは、STM32側からの`float`データを受信し、PC側から`float`データをSTM32に送信します。

```python
import serial
import struct
import time

# シリアルポートの設定（ポートは環境に合わせて変更）
port = '/dev/ttyACM0'  # Windowsの場合は 'COM3' のように指定
baud_rate = 115200
ser = serial.Serial(port, baud_rate, timeout=1)

# STM32からfloat配列を受信
def receive_float_array():
    while True:
        # ヘッダー 0xA5 を待つ
        if ser.read(1) == b'\xA5':
            # データ長を受信
            length = ser.read(1)
            if length:
                length = struct.unpack('B', length)[0]  # 1バイト（unsigned char）として長さを取得
                # float データを受信
                data = []
                for _ in range(length):
                    float_bytes = ser.read(4)
                    if len(float_bytes) == 4:
                        value = struct.unpack('f', float_bytes)[0]
                        data.append(value)
                return data

# PCからSTM32にfloat配列を送信
def send_float_array(data):
    header = b'\xA5'  # ヘッダー
    ser.write(header)  # ヘッダー送信
    length = len(data)
    ser.write(struct.pack('B', length))  # 配列の長さを送信
    for value in data:
        ser.write(struct.pack('f', value))  # 各floatを送信

if __name__ == "__main__":
    # STM32からデータを受信
    print("Waiting for data from STM32...")
    received_data = receive_float_array()
    print("Received float array from STM32:", received_data)

    # 1秒待機
    time.sleep(1)

    # PCからSTM32にデータを送信
    data_to_send = [9.87, 6.54, 3.21]
    print("Sending data to STM32:", data_to_send)
    send_float_array(data_to_send)
```

### 説明

- **STM32側 (`main.cpp`)**:
  - `AltairSerial`ライブラリを使って、`float`型の配列データを送信します。
  - 同時にPCからの`float`データを受信して、受信内容をシリアルモニターに表示します。
  
- **PC側（Python）**:
  - STM32から送られてくる`float`型データを受信して表示します。
  - その後、別の`float`型データをSTM32に送信します。

### 実行手順

1. **STM32マイコン側**：
   - `main.cpp`をコンパイルし、STM32F446REに書き込みます。
   - デバッグ用に、PlatformIOのシリアルモニターを起動することをお勧めします。

2. **PC側（Python）**：
   - Pythonスクリプトを実行し、STM32からデータを受信し、その後データを送信します。
   - 必要に応じてシリアルポートを変更してください（例: `COM3` や `/dev/ttyACM0`）。

### 動作確認

1. STM32側からPCにデータが送信され、Pythonプログラムがそのデータを受信して表示します。
2. PythonプログラムがデータをSTM32に送信し、STM32側で受信してデータが表示されます。

