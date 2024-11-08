#include "serial_lib.h"
#include <stdlib.h>

// シリアル通信の初期化
void Serial_Init(UART_HandleTypeDef *huart) {
    HAL_UART_Init(huart);
}

// 可変長データの送信関数
void Serial_SendData(UART_HandleTypeDef *huart, int16_t *data, uint8_t data_count) {
    uint8_t buffer_size = 2 + data_count * 2;
    uint8_t *buffer = (uint8_t *)malloc(buffer_size);

    buffer[0] = SERIAL_HEADER1;
    buffer[1] = SERIAL_HEADER2;

    for (uint8_t i = 0; i < data_count; i++) {
        buffer[2 + i * 2] = (data[i] >> 8) & 0xFF;
        buffer[3 + i * 2] = data[i] & 0xFF;
    }

    HAL_UART_Transmit(huart, buffer, buffer_size, HAL_MAX_DELAY);
    free(buffer);
}

// 可変長データの受信関数
uint8_t Serial_ReceiveData(UART_HandleTypeDef *huart, int16_t *data, uint8_t data_count) {
    uint8_t buffer_size = 2 + data_count * 2;
    uint8_t *buffer = (uint8_t *)malloc(buffer_size);

    if (HAL_UART_Receive(huart, buffer, buffer_size, HAL_MAX_DELAY) == HAL_OK) {
        if (buffer[0] == SERIAL_HEADER1 && buffer[1] == SERIAL_HEADER2) {
            for (uint8_t i = 0; i < data_count; i++) {
                data[i] = (buffer[2 + i * 2] << 8) | buffer[3 + i * 2];
            }
            free(buffer);
            return 1; // 正常受信
        }
    }
    free(buffer);
    return 0; // エラー
}
