# usart_lib.md

## USARTユーティリティ: `usart_lib`

`usart_lib` は、STM32 HALのUSART APIに対して、
Skenライブラリの使い勝手に近いユーティリティをCで提供します。

---

## 1. 概要

- 1バイト受信 + タイムアウトフラグ管理
- 送信ラッパ
- DMA受信開始（USART1/2/3 のデフォルト割当）

---

## 2. 必要ファイル

- `usart_lib.h`
- `usart_lib.c`

---

## 3. API

### 3.1 初期化

```c
void UsartLib_Init(UsartLib *usart, USART_HandleTypeDef *husart);
```

### 3.2 送信

```c
HAL_StatusTypeDef UsartLib_Write(UsartLib *usart, uint8_t *data, uint16_t size, uint32_t timeout_ms);
```

### 3.3 1バイト受信

```c
HAL_StatusTypeDef UsartLib_ReadByte(UsartLib *usart, uint8_t *data, uint32_t timeout_ms);
```

- タイムアウト時は `timeout_flag=1` になります。
- 内部で `HAL_USART_Init` とエラーフラグクリアを行います。

### 3.4 タイムアウト状態確認

```c
uint8_t UsartLib_CheckTimeout(UsartLib *usart);
void UsartLib_ClearTimeout(UsartLib *usart);
```

### 3.5 DMA受信開始

```c
HAL_StatusTypeDef UsartLib_StartDmaRead(UsartLib *usart, DMA_HandleTypeDef *hdma_rx,
                                        uint8_t *buffer, uint16_t size);
```

- 内部で以下を設定します。
  - 転送方向: Peripheral -> Memory
  - メモリインクリメント: ENABLE
  - モード: CIRCULAR
- デフォルト対応インスタンス:
  - `USART1` -> `DMA2_Stream2 / Channel4`
  - `USART2` -> `DMA1_Stream5 / Channel4`
  - `USART3` -> `DMA1_Stream1 / Channel4`

---

## 4. 使用例

### 4.1 ポーリング受信

```c
#include "Altair_library_for_CubeIDE/usart_lib.h"

extern USART_HandleTypeDef husart2;

int main(void)
{
    UsartLib usart;
    uint8_t rx;

    UsartLib_Init(&usart, &husart2);

    while (1) {
        if (UsartLib_ReadByte(&usart, &rx, 10) == HAL_OK) {
            (void)UsartLib_Write(&usart, &rx, 1, 10);
        }

        if (UsartLib_CheckTimeout(&usart)) {
            UsartLib_ClearTimeout(&usart);
        }
    }
}
```

### 4.2 DMA受信

```c
#include "Altair_library_for_CubeIDE/usart_lib.h"

extern USART_HandleTypeDef husart2;

int main(void)
{
    UsartLib usart;
    DMA_HandleTypeDef hdma_rx;
    uint8_t rx_buffer[64];

    UsartLib_Init(&usart, &husart2);
    UsartLib_StartDmaRead(&usart, &hdma_rx, rx_buffer, sizeof(rx_buffer));

    while (1) {
        HAL_Delay(10);
    }
}
```

---

## 5. 注意点

- 現在の実装は `USART_HandleTypeDef` ベースです。
- `UART_HandleTypeDef` を使う場合は `serial_lib` を利用してください。
- DMA IRQ設定はCubeMX側設定に依存します。DMAのNVIC設定も有効にしてください。
