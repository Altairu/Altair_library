
# Altair_library

## 概要
`Altair_library` は、mbed 用のライブラリ集です。ロボット制御やセンサーデータの取得を容易にするための多くの機能を提供します。

## 導入方法

### 1. ライブラリの配置
`Altair_library` フォルダーを PlatformIO プロジェクトの `lib` フォルダー内に配置します。配置後のディレクトリ構成は以下のようになります。

```
<Your_Project_Folder>
│
├── lib
│   └── Altair_library
│       ├── Altairlibrary.h
│       ├── encoder.h
│       ├── MotorDriver.h
│       ├── PIDController.h
│       ├── Servo.h
│       ├── encoder.cpp
│       ├── Servo.cpp
│       ├── Kinematics.h
│       ├── TwoWheelKinematics.h
│       ├── robot_control.h
│       ├── inverse_kinematics.h
│       ├── README.md
│       └── ...
│
└── src
    └── main.cpp
```
もしくは，
以下のように「lib_extra_dirs」を設定するとライブラリを使える．
> 注意：ライブラリの入ってるフォルダーの一個前を設定する

```
[env:nucleo_f446re]
platform = ststm32
board = nucleo_f446re
framework = mbed
lib_extra_dirs = /home/altair/OneDrive/lib
```

### 2. `main.cpp` での使用
プロジェクトの `src` フォルダー内にある `main.cpp` で、次のように `Altairlibrary.h` をインクルードしてください。

```cpp
#include "mbed.h"
#include "Altairlibrary.h"

int main() {
    // 初期化コードを書く

    while (true) {
        // メインループコードを書く
    }
}
```

## ライブラリ内容

`Altair_library` には以下のファイルが含まれています：

- **`Altairlibrary.h`**： 全てのヘッダーファイルをインクルードするマスターヘッダー
- **`encoder.h` / `encoder.cpp`**： エンコーダ用のライブラリ  
  ロータリーエンコーダを使用して、回転数や角度を計測する機能を提供します。
- **`MotorDriver.h`**： モータードライバー用のライブラリ  
  モーターの正転・逆転、PWM制御、ショートブレーキ機能をサポートしています。
- **`PIDController.h`**： PIDコントローラーライブラリ  
  PID制御を実装するための簡単なインターフェースを提供します。P、I、D ゲインを設定し、制御ループ内で PID 演算を行います。
- **`Servo.h` / `Servo.cpp`**： サーボモーター用のライブラリ  
  任意の角度でサーボを動かすための簡単なインターフェースを提供します。
- **`TwoWheelKinematics.h`**： 二輪運動学のライブラリ  
  二輪ロボットの前進・旋回動作を計算し、左右のモーター速度を制御するための機能を提供します。
- **`Kinematics.h`**： 足回りロボット運動学のライブラリ  
  四輪オムニ、三輪オムニ、四輪メカナムの運動学をサポートし、各ホイールの目標速度を計算します。
- **`robot_control.h`**： 足回りロボットの制御ライブラリ  
  上記の運動学ライブラリと PID コントローラーを組み合わせて、足回りロボットの制御を行います。スレッドを使った並列処理にも対応しています。
- **`inverse_kinematics.h`**： 自己位置推定ライブラリ  
  各ホイールのエンコーダデータからロボットの現在位置と姿勢を推定します。Omni3、Omni4の構成に対応しており、自己位置をリアルタイムで推定します。
- **`AltairSerial.h`**： シリアル通信ライブラリ  

各ライブラリの詳細な使用方法については、`readme` フォルダー内に個別の README を掲載していますので、そちらをご覧ください。また、`はじめて.md` には mbed の基礎的な書き方が記載されていますので、初心者の方はまずこちらを参照してください。

## ライセンス
Apache License 2.0

