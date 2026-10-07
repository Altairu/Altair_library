#include "can_lib.h"

// 受信キューの実体（外部から参照できるようにする）
CanRxQueue g_can1_rx_queue = {0};
CanRxQueue g_can2_rx_queue = {0};

// キューから1フレーム取り出す（メインループ側で呼ぶ）
uint8_t Can_Receive(CanRxQueue *queue, CanRxData *out) {
    uint16_t tail = queue->tail;

    if (tail == queue->head) {
        return 0;
    }

    *out = queue->buf[tail];
    queue->tail = (uint16_t)((tail + 1U) % CAN_RX_QUEUE_SIZE);
    return 1;
}

CanInitConfig Can_DefaultInitConfig(CAN_HandleTypeDef *hcan) {
    CanInitConfig config;

    config.fifo_assignment = CAN_FILTER_FIFO0;
    config.slave_start_filter_bank = 14;

    if (hcan->Instance == CAN1) {
        config.filter_bank = 0;
    } else {
        config.filter_bank = 14;
    }

    return config;
}

// フィルタ設定とCANの開始
HAL_StatusTypeDef Can_Init(CAN_HandleTypeDef *hcan, const CanInitConfig *config) {
    CAN_FilterTypeDef filter;
    CanInitConfig local_config;

    if (config == NULL) {
        local_config = Can_DefaultInitConfig(hcan);
        config = &local_config;
    }

    // 全てのIDを受信する設定
    filter.FilterIdHigh         = 0x0000;
    filter.FilterIdLow          = 0x0000;
    filter.FilterMaskIdHigh     = 0x0000;
    filter.FilterMaskIdLow      = 0x0000;
    filter.FilterFIFOAssignment = config->fifo_assignment;
    filter.FilterBank           = config->filter_bank;
    filter.SlaveStartFilterBank = config->slave_start_filter_bank;
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
    uint8_t rx_buf[8];
    CanRxQueue *queue = NULL;

    if (hcan->Instance == CAN1) {
        queue = &g_can1_rx_queue;
    } else if (hcan->Instance == CAN2) {
        queue = &g_can2_rx_queue;
    } else {
        return;
    }

    // FIFOに残っているフレームを可能な限りキューへ移す
    // キューが満杯でもFIFOを空けるため、必ず読み出す
    while (HAL_CAN_GetRxFifoFillLevel(hcan, CAN_RX_FIFO0) > 0U) {
        if (HAL_CAN_GetRxMessage(hcan, CAN_RX_FIFO0, &rx_header, rx_buf) != HAL_OK) {
            break;
        }

        uint16_t head = queue->head;
        uint16_t next = (uint16_t)((head + 1U) % CAN_RX_QUEUE_SIZE);

        if (next == queue->tail) {
            // キュー満杯：このフレームは破棄してカウントだけ残す
            queue->overflow_count++;
            continue;
        }

        CanRxData *slot = &queue->buf[head];
        slot->std_id = rx_header.StdId;
        slot->dlc    = rx_header.DLC;
        for (uint8_t i = 0; i < 8; i++) {
            slot->data[i] = rx_buf[i];
        }

        // データ書き込み後にheadを進める（メイン側はheadを見てから読む）
        queue->head = next;
    }
}
