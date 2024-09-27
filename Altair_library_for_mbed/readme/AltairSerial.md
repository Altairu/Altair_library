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
