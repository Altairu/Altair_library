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
} CanRxData;

// 初期化パラメータ（デュアルCAN時のフィルタ割り当て用）
typedef struct {
    uint32_t fifo_assignment;      // CAN_FILTER_FIFO0 または CAN_FILTER_FIFO1
    uint32_t filter_bank;          // 使用するフィルタバンク番号
    uint32_t slave_start_filter_bank; // デュアルCAN時の分割開始バンク（単体CAN時は無視）
} CanInitConfig;

// 受信キューの深さ（2のべき乗でなくてもよい）
#define CAN_RX_QUEUE_SIZE  32

// 受信キュー（割り込みが書き込み側、メインループが読み出し側の単一生産者/単一消費者リングバッファ）
typedef struct {
    CanRxData         buf[CAN_RX_QUEUE_SIZE];
    volatile uint16_t head;            // 次に書き込む位置（割り込みのみ更新）
    volatile uint16_t tail;            // 次に読み出す位置（メインループのみ更新）
    volatile uint32_t overflow_count;  // キュー満杯で破棄したフレーム数
} CanRxQueue;

extern CanRxQueue g_can1_rx_queue;
extern CanRxQueue g_can2_rx_queue;

// 受信キューから1フレーム取り出す（取り出せたら1、空なら0を返す）
uint8_t Can_Receive(CanRxQueue *queue, CanRxData *out);

// 関数プロトタイプ
HAL_StatusTypeDef Can_Init(CAN_HandleTypeDef *hcan, const CanInitConfig *config);
HAL_StatusTypeDef Can_Transmit(CAN_HandleTypeDef *hcan, uint32_t std_id, uint8_t *pData, uint8_t size);

// デフォルト設定ヘルパ
CanInitConfig Can_DefaultInitConfig(CAN_HandleTypeDef *hcan);

#endif /* CAN_LIB_H */
