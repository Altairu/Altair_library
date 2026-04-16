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

// 初期化パラメータ（デュアルCAN時のフィルタ割り当て用）
typedef struct {
    uint32_t fifo_assignment;      // CAN_FILTER_FIFO0 または CAN_FILTER_FIFO1
    uint32_t filter_bank;          // 使用するフィルタバンク番号
    uint32_t slave_start_filter_bank; // デュアルCAN時の分割開始バンク（単体CAN時は無視）
} CanInitConfig;

extern CanRxData g_can1_rx_data;
extern CanRxData g_can2_rx_data;

// 関数プロトタイプ
HAL_StatusTypeDef Can_Init(CAN_HandleTypeDef *hcan, const CanInitConfig *config);
HAL_StatusTypeDef Can_Transmit(CAN_HandleTypeDef *hcan, uint32_t std_id, uint8_t *pData, uint8_t size);

// デフォルト設定ヘルパ
CanInitConfig Can_DefaultInitConfig(CAN_HandleTypeDef *hcan);

#endif /* CAN_LIB_H */
