# Altair_library_for_CubeIDE

STM32CubeIDE (CMake プロジェクト) 向けライブラリです。

## 含まれるライブラリ

| ファイル | 概要 | 詳細ドキュメント |
|---|---|---|
| `can_lib` | CAN 通信 | [readme/can_lib.md](readme/can_lib.md) |
| `encoder` | エンコーダ | [readme/encoder.md](readme/encoder.md) |
| `gpio_lib` | GPIO/PWM ユーティリティ | [readme/gpio_lib.md](readme/gpio_lib.md) |
| `kinematics` | 運動学 | [readme/kinematics.md](readme/kinematics.md) |
| `motor_driver` | モータドライバ | [readme/motor_driver.md](readme/motor_driver.md) |
| `pid` | PID 制御 | [readme/pid.md](readme/pid.md) |
| `serial_lib` | シリアル通信 | [readme/Serial.md](readme/Serial.md) |
| `usart_lib` | USART 通信ユーティリティ | [readme/usart_lib.md](readme/usart_lib.md) |

## 導入手順

### 1. リポジトリをダウンロード

[https://github.com/Altairu/Altair_library](https://github.com/Altairu/Altair_library) よりダウンロードします。

![ダウンロード](images/１.png)

### 2. フォルダをコピー

`Altair_library_for_CubeIDE` フォルダを選択してコピーします。

![フォルダ選択](images/２.png)

### 3. プロジェクトに配置

STM32CubeIDE プロジェクトの `Core/Inc` 内に貼り付けます。

```
Core/
└── Inc/
    └── Altair_library_for_CubeIDE/   ← ここに配置
        ├── altair.h
        ├── can_lib.h / can_lib.c
        ├── encoder.h / encoder.c
        ├── gpio_lib.h / gpio_lib.c
        ├── kinematics.h / kinematics.c
        ├── motor_driver.h / motor_driver.c
        ├── pid.h / pid.c
        ├── serial_lib.h / serial_lib.c
        └── usart_lib.h / usart_lib.c
```

![配置](images/３.png)

### 4. CMakeLists.txt を編集

`cmake/stm32cubemx/CMakeLists.txt` を以下のように編集します。

```cmake
# STM32CubeMX generated include paths
set(MX_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc/Altair_library_for_CubeIDE  # ← 追加
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/STM32F4xx_HAL_Driver/Inc/Legacy
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Device/ST/STM32F4xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Drivers/CMSIS/Include
)

# STM32CubeMX generated application sources
set(MX_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f4xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/stm32f4xx_hal_msp.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/sysmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Src/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../startup_stm32f446xx.s
    # ↓ 使うライブラリの .c を追加
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc/Altair_library_for_CubeIDE/can_lib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc/Altair_library_for_CubeIDE/encoder.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc/Altair_library_for_CubeIDE/gpio_lib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc/Altair_library_for_CubeIDE/kinematics.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc/Altair_library_for_CubeIDE/motor_driver.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc/Altair_library_for_CubeIDE/pid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc/Altair_library_for_CubeIDE/serial_lib.c
    ${CMAKE_CURRENT_SOURCE_DIR}/../../Core/Inc/Altair_library_for_CubeIDE/usart_lib.c
)
```

![CMakeLists.txt](images/４.png)

![ビルド設定](images/image.png)

### 5. main.c にインクルード

`main.c` の先頭に以下を追加するだけで全ライブラリが使用可能になります。

```c
#include "Altair_library_for_CubeIDE/altair.h"
```

---

## CAN 通信を使う場合の追加設定

CubeMX で以下を設定しないと通信が不安定になったり止まったりします。

| パラメータ | 推奨値 |
|---|---|
| `AutoRetransmission` | `ENABLE` |
| `AutoBusOff` | `ENABLE` |
| `SyncJumpWidth` | `CAN_SJW_4TQ` |

詳細は [readme/can_lib.md](readme/can_lib.md) を参照してください。

### ハードウェア注意事項

- **終端抵抗**: バスの両端に **120Ω** が必要。ないとビットエラーが多発して通信が不安定になる。
- **ボーレート**: 全ノードで一致していること。ボーレートの計算式は can_lib.md を参照。
- **ACK**: バスに自分以外のノードが最低1つ存在しないと ACK エラーになり Bus-Off に至る。単体テスト時は CubeMX で `Mode = CAN_MODE_LOOPBACK` に設定する。
