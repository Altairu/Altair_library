#include "serial_lib.h"

// シリアル通信の初期化
void Serial_Init(UART_HandleTypeDef *huart) {
    HAL_UART_Init(huart);
}

// データ送信関数: 3つの16ビット整数データをヘッダーと共に送信
void Serial_SendData(UART_HandleTypeDef *huart, int16_t data1, int16_t data2, int16_t data3) {
    uint8_t buffer[8];
    buffer[0] = SERIAL_HEADER1;
    buffer[1] = SERIAL_HEADER2;
    buffer[2] = (data1 >> 8) & 0xFF;
    buffer[3] = data1 & 0xFF;
    buffer[4] = (data2 >> 8) & 0xFF;
    buffer[5] = data2 & 0xFF;
    buffer[6] = (data3 >> 8) & 0xFF;
    buffer[7] = data3 & 0xFF;

    HAL_UART_Transmit(huart, buffer, 8, HAL_MAX_DELAY);
}

// データ受信関数: ヘッダー確認後に3つのデータを受信
uint8_t Serial_ReceiveData(UART_HandleTypeDef *huart, int16_t *data1, int16_t *data2, int16_t *data3) {
    uint8_t buffer[8];

    // 8バイトの受信: ヘッダー2バイト + データ6バイト
    if (HAL_UART_Receive(huart, buffer, 8, HAL_MAX_DELAY) == HAL_OK) {
        // ヘッダー確認
        if (buffer[0] == SERIAL_HEADER1 && buffer[1] == SERIAL_HEADER2) {
            *data1 = (buffer[2] << 8) | buffer[3];
            *data2 = (buffer[4] << 8) | buffer[5];
            *data3 = (buffer[6] << 8) | buffer[7];
            return 1; // 正常受信
        }
    }
    return 0; // エラー
}
