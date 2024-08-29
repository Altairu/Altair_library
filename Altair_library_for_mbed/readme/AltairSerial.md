
# AltairSerial

AltairSerialは、Mbedプラットフォームで動作する自作のソフトウェアシリアルライブラリです。このライブラリは、指定したピンでソフトウェアシリアル通信を行うことができ、`float`型や`char`型のデータを送受信する機能を提供します。

## 特徴
- USBモード（USB_A、USB_B、USB_MiniB）に対応
- データの送信時に自動で先頭に`0xA5`を追加
- `float`型や`char`型の配列データを送受信可能
- ボーレートはデフォルトで9600bps（設定可能）

## 対応ピン
- **USB_A**: `PA_9` (TX), `PA_10` (RX)
- **USB_B**: `PC_10` (TX), `PC_11` (RX)
- **USB_MiniB**: `PA_2` (TX), `PA_3` (RX)

## 使用方法

### 初期化

```cpp
#include "AltairSerial.h"

AltairSerial serial(USB_B, 9600); // USB_Bモードで初期化、ボーレート9600bps
```

### char型データの送受信

#### 送信

```cpp
serial.sendWithHeader(42); // 先頭に0xA5を付けて42を送信
```

#### 受信

```cpp
char received = serial.receiveWithHeader();
if (received != -1) { // 受信成功時
    printf("Received: %d\n", received);
}
```

### float型配列データの送受信

#### 送信

```cpp
float dataToSend[] = {10.5, 20.75, 30.125};
serial.sendFloatArrayWithHeader(dataToSend, sizeof(dataToSend) / sizeof(float));
```

#### 受信

```cpp
float receivedData[10];
int length;
serial.receiveFloatArrayWithHeader(receivedData, length);

for (int i = 0; i < length; i++) {
    printf("%f ", receivedData[i]);
}
printf("\n");
```

#### Python

```py
import serial
import struct

# シリアルポートの設定
port = '/dev/ttyACM0'  # 適切なポート名に変更してください
baud_rate = 9600
ser = serial.Serial(port, baud_rate, timeout=1)

def receive_float_array():
    while True:
        # ヘッダー 0xA5 を探す
        if ser.read(1) == b'\xA5':
            # データ長を受信
            length = ser.read(1)
            if length:
                length = struct.unpack('B', length)[0]  # 1バイト（unsigned char）として長さを取得

                # float データを受信
                data = []
                for _ in range(length):
                    # 各 float は 4 バイトで構成される
                    float_bytes = ser.read(4)
                    if len(float_bytes) == 4:
                        value = struct.unpack('f', float_bytes)[0]
                        # 小数点第4位で四捨五入
                        value = round(value, 4)
                        data.append(value)
                    else:
                        print("データ受信エラー")
                        break

                return data

def main():
    while True:
        # float 配列を受信
        float_array = receive_float_array()
        if float_array:
            print("Received float array:", float_array)

if __name__ == '__main__':
    main()

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
void sendWithHeader(char data);
void sendFloatArrayWithHeader(float* data, int length);
```

- **sendWithHeader(char data)**: 先頭に`0xA5`を付けて単一のデータを送信します。
- **sendFloatArrayWithHeader(float* data, int length)**: `float`型の配列を送信します。先頭に`0xA5`と配列の長さを自動で追加します。

### データ受信

```cpp
char receiveWithHeader();
void receiveFloatArrayWithHeader(float* buffer, int& length);
```

- **receiveWithHeader()**: `0xA5`を認識した後、次のデータを受信します。
- **receiveFloatArrayWithHeader(float* buffer, int& length)**: `0xA5`を認識し、`float`型配列データを受信します。

### その他
必要に応じて、`platformio.ini`ファイルに以下の設定を追加し、`float`型や`long`型のデータをサポートします。

```ini
build_flags = 
    -D PIO_FRAMEWORK_MBED_RTOS_PRESENT
    -D MBED_MINIMAL_PRINTF_ENABLE_FLOATING_POINT
    -D MBED_MINIMAL_PRINTF_ENABLE_64_BIT
```
