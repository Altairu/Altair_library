#ifndef SERIAL_LIB_H
#define SERIAL_LIB_H

#include "main.h"

// シリアルヘッダーの定義
#define SERIAL_HEADER1 0xA5
#define SERIAL_HEADER2 0xA5

// 1回の送受信で扱える最大データ数（int16_t単位）
#define SERIAL_MAX_DATA_COUNT 16

// 関数プロトタイプ
void Serial_Init(USART_HandleTypeDef *huart);
void Serial_SendData(USART_HandleTypeDef *huart, int16_t *data, uint8_t data_count);
uint8_t Serial_ReceiveData(USART_HandleTypeDef *huart, int16_t *data, uint8_t data_count);

#endif // SERIAL_LIB_H
