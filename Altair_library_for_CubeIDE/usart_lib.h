#ifndef USART_LIB_H
#define USART_LIB_H

#include "stm32f4xx_hal.h"

typedef struct {
    USART_HandleTypeDef *husart;
    uint8_t timeout_flag;
} UsartLib;

void UsartLib_Init(UsartLib *usart, USART_HandleTypeDef *husart);
HAL_StatusTypeDef UsartLib_Write(UsartLib *usart, uint8_t *data, uint16_t size, uint32_t timeout_ms);
HAL_StatusTypeDef UsartLib_ReadByte(UsartLib *usart, uint8_t *data, uint32_t timeout_ms);
uint8_t UsartLib_CheckTimeout(UsartLib *usart);
void UsartLib_ClearTimeout(UsartLib *usart);

HAL_StatusTypeDef UsartLib_ConfigRxDmaDefault(USART_HandleTypeDef *husart, DMA_HandleTypeDef *hdma_rx);
HAL_StatusTypeDef UsartLib_StartDmaRead(UsartLib *usart, DMA_HandleTypeDef *hdma_rx,
                                        uint8_t *buffer, uint16_t size);

#endif /* USART_LIB_H */
