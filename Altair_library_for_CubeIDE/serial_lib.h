#ifndef SERIAL_LIB_H
#define SERIAL_LIB_H

#include "main.h"

// シリアルヘッダーの定義
#define SERIAL_HEADER1 0xA5
#define SERIAL_HEADER2 0xA5

// 関数プロトタイプ
void Serial_Init(UART_HandleTypeDef *huart);
void Serial_SendData(UART_HandleTypeDef *huart, int16_t *data, uint8_t data_count);
uint8_t Serial_ReceiveData(UART_HandleTypeDef *huart, int16_t *data, uint8_t data_count);

#endif // SERIAL_LIB_H
