#ifndef GPIO_LIB_H
#define GPIO_LIB_H

#include "stm32f4xx_hal.h"

typedef enum {
    GPIO_LIB_MODE_OUTPUT,
    GPIO_LIB_MODE_INPUT,
    GPIO_LIB_MODE_INPUT_PULLUP,
    GPIO_LIB_MODE_INPUT_PULLDOWN
} GpioLibMode;

typedef struct {
    GPIO_TypeDef *port;
    uint16_t pin;
    GpioLibMode mode;
    TIM_HandleTypeDef *htim;
    uint32_t channel;
    uint32_t pwm_period;
} GpioLib;

HAL_StatusTypeDef GpioLib_Init(GpioLib *gpio, GPIO_TypeDef *port, uint16_t pin, GpioLibMode mode);
HAL_StatusTypeDef GpioLib_InitPwm(GpioLib *gpio, GPIO_TypeDef *port, uint16_t pin,
                                  TIM_HandleTypeDef *htim, uint32_t channel);
void GpioLib_Write(GpioLib *gpio, GPIO_PinState state);
void GpioLib_Toggle(GpioLib *gpio);
GPIO_PinState GpioLib_Read(GpioLib *gpio);
void GpioLib_WriteDuty(GpioLib *gpio, float duty_percent);

#endif /* GPIO_LIB_H */
