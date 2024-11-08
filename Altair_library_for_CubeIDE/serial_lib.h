#ifndef SERIAL_LIB_H
#define SERIAL_LIB_H

#include "main.h"

// シリアルヘッダーの定義
#define SERIAL_HEADER1 0xA5
#define SERIAL_HEADER2 0xA5

// 送信データの長さ
#define SERIAL_DATA_LENGTH 3

// 関数プロトタイプ
void Serial_Init(UART_HandleTypeDef *huart);
void Serial_SendData(UART_HandleTypeDef *huart, int16_t data1, int16_t data2, int16_t data3);
uint8_t Serial_ReceiveData(UART_HandleTypeDef *huart, int16_t *data1, int16_t *data2, int16_t *data3);

#endif // SERIAL_LIB_H
