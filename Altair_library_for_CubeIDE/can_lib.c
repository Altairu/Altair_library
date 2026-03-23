#include "can_lib.h"

// 受信データの実体（外部から参照できるようにする）
CanRxData g_can1_rx_data = {0};

// フィルタ設定とCANの開始
HAL_StatusTypeDef Can_Init(CAN_HandleTypeDef *hcan) {
    CAN_FilterTypeDef filter;

    // 全てのIDを受信する設定
    filter.FilterIdHigh         = 0x0000;
    filter.FilterIdLow          = 0x0000;
    filter.FilterMaskIdHigh     = 0x0000;
    filter.FilterMaskIdLow      = 0x0000;
    filter.FilterFIFOAssignment = CAN_FILTER_FIFO0;
    filter.FilterBank           = 0;
    filter.FilterMode           = CAN_FILTERMODE_IDMASK;
    filter.FilterScale          = CAN_FILTERSCALE_32BIT;
    filter.FilterActivation     = CAN_FILTER_ENABLE;

    if (HAL_CAN_ConfigFilter(hcan, &filter) != HAL_OK) return HAL_ERROR;
    if (HAL_CAN_Start(hcan) != HAL_OK) return HAL_ERROR;

    // 受信割り込み（FIFO0メッセージ待機）を有効化
    if (HAL_CAN_ActivateNotification(hcan, CAN_IT_RX_FIFO0_MSG_PENDING) != HAL_OK) return HAL_ERROR;

    return HAL_OK;
}

// 送信関数（空きメールボックス待機・タイムアウト付き）
HAL_StatusTypeDef Can_Transmit(CAN_HandleTypeDef *hcan, uint32_t std_id, uint8_t *pData, uint8_t size) {
    CAN_TxHeaderTypeDef tx_header;
    uint32_t tx_mailbox;
    uint32_t deadline = HAL_GetTick() + CAN_TX_TIMEOUT_MS;

    // 空きメールボックスができるまで待つ
    while (HAL_CAN_GetTxMailboxesFreeLevel(hcan) == 0) {
        if (HAL_GetTick() >= deadline) {
            // タイムアウト：詰まった古い送信要求を全キャンセルして新しいデータを送る
            HAL_CAN_AbortTxRequest(hcan, CAN_TX_MAILBOX0 | CAN_TX_MAILBOX1 | CAN_TX_MAILBOX2);
            break;
        }
    }

    tx_header.StdId              = std_id;
    tx_header.RTR                = CAN_RTR_DATA;
    tx_header.IDE                = CAN_ID_STD;
    tx_header.DLC                = size;
    tx_header.TransmitGlobalTime = DISABLE;

    return HAL_CAN_AddTxMessage(hcan, &tx_header, pData, &tx_mailbox);
}

// HALの受信完了コールバックをオーバーライド
void HAL_CAN_RxFifo0MsgPendingCallback(CAN_HandleTypeDef *hcan) {
    CAN_RxHeaderTypeDef rx_header;
    if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, g_can1_rx_data.data) == HAL_OK) {
        g_can1_rx_data.std_id      = rx_header.StdId;
        g_can1_rx_data.dlc         = rx_header.DLC;
        g_can1_rx_data.new_data_flag = 1;
    }
}
