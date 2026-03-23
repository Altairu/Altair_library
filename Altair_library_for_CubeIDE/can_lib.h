#ifndef CAN_LIB_H
#define CAN_LIB_H

#include "stm32f4xx_hal.h"

// 送信タイムアウト[ms]（メールボックスが空くまでの最大待機時間）
#define CAN_TX_TIMEOUT_MS  10

// 受信データを管理する構造体
typedef struct {
    uint32_t std_id;         // スタンダードID
    uint8_t  data[8];        // データ本体
    uint8_t  dlc;            // データ長
    uint8_t  new_data_flag;  // 受信完了フラグ（読み取り後に0クリアすること）
} CanRxData;

extern CanRxData g_can1_rx_data;

// 関数プロトタイプ
HAL_StatusTypeDef Can_Init(CAN_HandleTypeDef *hcan);
HAL_StatusTypeDef Can_Transmit(CAN_HandleTypeDef *hcan, uint32_t std_id, uint8_t *pData, uint8_t size);

#endif /* CAN_LIB_H */
