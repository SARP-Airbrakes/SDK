#pragma once
#include "base.h"
#include "pinmap.h"

namespace sdk {

class DigitalOut {
public:
    explicit DigitalOut(Pin pin, int value = 0) {
        _pin = pin;

        // Enable GPIO Clock
        if (_pin.port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
        else if (_pin.port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
        else if (_pin.port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
        else if (_pin.port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
        // Extend as needed...

        GPIO_InitTypeDef init{};
        init.Pin = (1 << _pin.pin);
        init.Mode = GPIO_MODE_OUTPUT_PP;
        init.Pull = GPIO_NOPULL;
        init.Speed = GPIO_SPEED_FREQ_LOW;
        HAL_GPIO_Init(_pin.port, &init);

        write(value);
    }

    void write(int value) {
        HAL_GPIO_WritePin(_pin.port, (1 << _pin.pin), value ? GPIO_PIN_SET : GPIO_PIN_RESET);
    }

    int read() const {
        return HAL_GPIO_ReadPin(_pin.port, (1 << _pin.pin));
    }

    DigitalOut& operator= (int value) {
        write(value);
        return *this;
    }

    operator int() const {
        return read();
    }

private:
    Pin _pin;
};

} // namespace sdk
