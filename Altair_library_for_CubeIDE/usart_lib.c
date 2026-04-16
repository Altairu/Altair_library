#include "usart_lib.h"

static void UsartLib_ClearErrorFlags(USART_HandleTypeDef *husart)
{
    __HAL_USART_CLEAR_PEFLAG(husart);
    __HAL_USART_CLEAR_FEFLAG(husart);
    __HAL_USART_CLEAR_NEFLAG(husart);
    __HAL_USART_CLEAR_OREFLAG(husart);
    __HAL_USART_CLEAR_IDLEFLAG(husart);
}

void UsartLib_Init(UsartLib *usart, USART_HandleTypeDef *husart)
{
    if (usart == NULL) {
        return;
    }
    usart->husart = husart;
    usart->timeout_flag = 0U;
}

HAL_StatusTypeDef UsartLib_Write(UsartLib *usart, uint8_t *data, uint16_t size, uint32_t timeout_ms)
{
    if (usart == NULL || usart->husart == NULL || data == NULL) {
        return HAL_ERROR;
    }
    return HAL_USART_Transmit(usart->husart, data, size, timeout_ms);
}

HAL_StatusTypeDef UsartLib_ReadByte(UsartLib *usart, uint8_t *data, uint32_t timeout_ms)
{
    HAL_StatusTypeDef status;

    if (usart == NULL || usart->husart == NULL || data == NULL) {
        return HAL_ERROR;
    }

    status = HAL_USART_Receive(usart->husart, data, 1U, timeout_ms);
    if (status == HAL_TIMEOUT) {
        usart->timeout_flag = 1U;
        (void)HAL_USART_Init(usart->husart);
        UsartLib_ClearErrorFlags(usart->husart);
        *data = 0U;
    } else {
        usart->timeout_flag = 0U;
    }

    return status;
}

uint8_t UsartLib_CheckTimeout(UsartLib *usart)
{
    if (usart == NULL) {
        return 0U;
    }
    return usart->timeout_flag;
}

void UsartLib_ClearTimeout(UsartLib *usart)
{
    if (usart == NULL) {
        return;
    }
    usart->timeout_flag = 0U;
}

HAL_StatusTypeDef UsartLib_ConfigRxDmaDefault(USART_HandleTypeDef *husart, DMA_HandleTypeDef *hdma_rx)
{
    if (husart == NULL || hdma_rx == NULL) {
        return HAL_ERROR;
    }

    if (husart->Instance == USART1) {
        hdma_rx->Instance = DMA2_Stream2;
        hdma_rx->Init.Channel = DMA_CHANNEL_4;
    } else if (husart->Instance == USART2) {
        hdma_rx->Instance = DMA1_Stream5;
        hdma_rx->Init.Channel = DMA_CHANNEL_4;
    } else if (husart->Instance == USART3) {
        hdma_rx->Instance = DMA1_Stream1;
        hdma_rx->Init.Channel = DMA_CHANNEL_4;
    } else {
        return HAL_ERROR;
    }

    hdma_rx->Init.Direction = DMA_PERIPH_TO_MEMORY;
    hdma_rx->Init.PeriphInc = DMA_PINC_DISABLE;
    hdma_rx->Init.MemInc = DMA_MINC_ENABLE;
    hdma_rx->Init.PeriphDataAlignment = DMA_PDATAALIGN_BYTE;
    hdma_rx->Init.MemDataAlignment = DMA_MDATAALIGN_BYTE;
    hdma_rx->Init.Mode = DMA_CIRCULAR;
    hdma_rx->Init.Priority = DMA_PRIORITY_LOW;
    hdma_rx->Init.FIFOMode = DMA_FIFOMODE_DISABLE;

    return HAL_DMA_Init(hdma_rx);
}

HAL_StatusTypeDef UsartLib_StartDmaRead(UsartLib *usart, DMA_HandleTypeDef *hdma_rx,
                                        uint8_t *buffer, uint16_t size)
{
    HAL_StatusTypeDef status;

    if (usart == NULL || usart->husart == NULL || hdma_rx == NULL || buffer == NULL || size == 0U) {
        return HAL_ERROR;
    }

    if (UsartLib_ConfigRxDmaDefault(usart->husart, hdma_rx) != HAL_OK) {
        return HAL_ERROR;
    }

    __HAL_LINKDMA(usart->husart, hdmarx, *hdma_rx);
    status = HAL_USART_Receive_DMA(usart->husart, buffer, size);
    return status;
}
