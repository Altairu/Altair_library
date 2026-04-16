#include "serial_lib.h"

// シリアル通信の初期化（HAL_USART_Init はCubeMX生成コードで実施済みのため不要）
void Serial_Init(USART_HandleTypeDef *huart) {
    (void)huart; // CubeMXが初期化済み
}

// 可変長データの送信関数（固定バッファ使用、mallocなし）
void Serial_SendData(USART_HandleTypeDef *huart, int16_t *data, uint8_t data_count) {
    if (data_count > SERIAL_MAX_DATA_COUNT) {
        data_count = SERIAL_MAX_DATA_COUNT;
    }
    // スタック上の固定バッファ（ヘッダ2バイト + データ最大32バイト）
    uint8_t buffer[2 + SERIAL_MAX_DATA_COUNT * 2];
    uint8_t buffer_size = 2 + data_count * 2;

    buffer[0] = SERIAL_HEADER1;
    buffer[1] = SERIAL_HEADER2;

    for (uint8_t i = 0; i < data_count; i++) {
        buffer[2 + i * 2] = (uint8_t)((data[i] >> 8) & 0xFF);
        buffer[3 + i * 2] = (uint8_t)(data[i] & 0xFF);
    }

    HAL_USART_Transmit(huart, buffer, buffer_size, HAL_MAX_DELAY);
}

// 可変長データの受信関数（固定バッファ使用、mallocなし）
uint8_t Serial_ReceiveData(USART_HandleTypeDef *huart, int16_t *data, uint8_t data_count) {
    if (data_count > SERIAL_MAX_DATA_COUNT) {
        data_count = SERIAL_MAX_DATA_COUNT;
    }
    uint8_t buffer[2 + SERIAL_MAX_DATA_COUNT * 2];
    uint8_t buffer_size = 2 + data_count * 2;

    if (HAL_USART_Receive(huart, buffer, buffer_size, HAL_MAX_DELAY) == HAL_OK) {
        if (buffer[0] == SERIAL_HEADER1 && buffer[1] == SERIAL_HEADER2) {
            for (uint8_t i = 0; i < data_count; i++) {
                data[i] = (int16_t)((buffer[2 + i * 2] << 8) | buffer[3 + i * 2]);
            }
            return 1; // 正常受信
        }
    }
    return 0; // エラー
}
