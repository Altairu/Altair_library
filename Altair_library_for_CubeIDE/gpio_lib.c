#include "gpio_lib.h"

HAL_StatusTypeDef GpioLib_Init(GpioLib *gpio, GPIO_TypeDef *port, uint16_t pin, GpioLibMode mode)
{
    GPIO_InitTypeDef init = {0};

    if (gpio == NULL || port == NULL) {
        return HAL_ERROR;
    }

    gpio->port = port;
    gpio->pin = pin;
    gpio->mode = mode;
    gpio->htim = NULL;
    gpio->channel = 0U;
    gpio->pwm_period = 0U;

    init.Pin = pin;
    init.Speed = GPIO_SPEED_FREQ_HIGH;

    switch (mode) {
    case GPIO_LIB_MODE_OUTPUT:
        init.Mode = GPIO_MODE_OUTPUT_PP;
        init.Pull = GPIO_NOPULL;
        break;
    case GPIO_LIB_MODE_INPUT:
        init.Mode = GPIO_MODE_INPUT;
        init.Pull = GPIO_NOPULL;
        break;
    case GPIO_LIB_MODE_INPUT_PULLUP:
        init.Mode = GPIO_MODE_INPUT;
        init.Pull = GPIO_PULLUP;
        break;
    case GPIO_LIB_MODE_INPUT_PULLDOWN:
        init.Mode = GPIO_MODE_INPUT;
        init.Pull = GPIO_PULLDOWN;
        break;
    default:
        return HAL_ERROR;
    }

    HAL_GPIO_Init(port, &init);
    return HAL_OK;
}

HAL_StatusTypeDef GpioLib_InitPwm(GpioLib *gpio, GPIO_TypeDef *port, uint16_t pin,
                                  TIM_HandleTypeDef *htim, uint32_t channel)
{
    if (gpio == NULL || port == NULL || htim == NULL) {
        return HAL_ERROR;
    }

    gpio->port = port;
    gpio->pin = pin;
    gpio->mode = GPIO_LIB_MODE_OUTPUT;
    gpio->htim = htim;
    gpio->channel = channel;
    gpio->pwm_period = __HAL_TIM_GET_AUTORELOAD(htim);

    if (HAL_TIM_PWM_Start(htim, channel) != HAL_OK) {
        return HAL_ERROR;
    }

    __HAL_TIM_SET_COMPARE(htim, channel, 0U);
    return HAL_OK;
}

void GpioLib_Write(GpioLib *gpio, GPIO_PinState state)
{
    if (gpio == NULL || gpio->port == NULL) {
        return;
    }
    HAL_GPIO_WritePin(gpio->port, gpio->pin, state);
}

void GpioLib_Toggle(GpioLib *gpio)
{
    if (gpio == NULL || gpio->port == NULL) {
        return;
    }
    HAL_GPIO_TogglePin(gpio->port, gpio->pin);
}

GPIO_PinState GpioLib_Read(GpioLib *gpio)
{
    if (gpio == NULL || gpio->port == NULL) {
        return GPIO_PIN_RESET;
    }
    return HAL_GPIO_ReadPin(gpio->port, gpio->pin);
}

void GpioLib_WriteDuty(GpioLib *gpio, float duty_percent)
{
    uint32_t compare;

    if (gpio == NULL || gpio->htim == NULL) {
        return;
    }

    if (duty_percent < 0.0f) {
        duty_percent = 0.0f;
    }
    if (duty_percent > 100.0f) {
        duty_percent = 100.0f;
    }

    gpio->pwm_period = __HAL_TIM_GET_AUTORELOAD(gpio->htim);
    compare = (uint32_t)((duty_percent / 100.0f) * (float)gpio->pwm_period);
    __HAL_TIM_SET_COMPARE(gpio->htim, gpio->channel, compare);
}
