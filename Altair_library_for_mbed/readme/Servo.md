# Servo ライブラリ

## 概要
`Servo` ライブラリは、サーボモーターを制御するための簡単なライブラリです。サーボモーターの角度を指定して制御することができます。

## 使用方法

### 初期化

```cpp
#include "Servo.h"

// サーボモーターが接続されているピンを指定して初期化する
Servo servo(PD_9);
```

### メソッド

- `void write(float angle)`: サーボモーターの角度を設定します。角度は 0 度から 180 度の範囲で指定します。

### サンプルコード

```cpp
#include "mbed.h"
#include "Servo.h"

Servo servo(PD_9);

int main() {
    while (true) {
        servo.write(0.0);   // サーボを0度に設定
        ThisThread::sleep_for(1s);

        servo.write(90.0);  // サーボを90度に設定
        ThisThread::sleep_for(1s);

        servo.write(180.0); // サーボを180度に設定
        ThisThread::sleep_for(1s);
    }
}
```


