#pragma once
#include "base.hpp"
#include "pinmap.hpp"

namespace sdk {

class DigitalIn {
public:
    explicit DigitalIn(Pin pin, GPIOPuPd_TypeDef pull = GPIO_NOPULL) {
        _pin = pin;

        // Enable GPIO Clock
        if (_pin.port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
        else if (_pin.port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
        else if (_pin.port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
        else if (_pin.port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
        // Extend as needed...

        GPIO_InitTypeDef init{};
        init.Pin = (1 << _pin.pin);
        init.Mode = GPIO_MODE_INPUT;
        init.Pull = pull;
        init.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(_pin.port, &init);
    }

    int read() const {
        return HAL_GPIO_ReadPin(_pin.port, (1 << _pin.pin));
    }

    operator int() const {
        return read();
    }

    int is_connected() const {
        return (_pin.port != nullptr);
    }

private:
    Pin _pin;
};

} // namespace sdk
