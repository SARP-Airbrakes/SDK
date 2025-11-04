#pragma once
#include "base.h"
#include "pinmap.h"

namespace sdk {

enum class PinDirection {
    In,
    Out
};

enum class PinMode {
    PullNone = GPIO_NOPULL,
    PullUp   = GPIO_PULLUP,
    PullDown = GPIO_PULLDOWN
};

class DigitalInOut {
public:
    // Constructors
    DigitalInOut(Pin pin)
        : _pin(pin), _direction(PinDirection::In), _mode(PinMode::PullNone)
    {
        enable_gpio_clock(_pin.port);
        configure(PinDirection::In, PinMode::PullNone, 0);
    }

    DigitalInOut(Pin pin, PinDirection direction, PinMode mode, int value = 0)
        : _pin(pin), _direction(direction), _mode(mode)
    {
        enable_gpio_clock(_pin.port);
        configure(direction, mode, value);
    }

    // Core methods
    void write(int value) {
        if (_direction == PinDirection::Out) {
            HAL_GPIO_WritePin(_pin.port, (1 << _pin.pin), value ? GPIO_PIN_SET : GPIO_PIN_RESET);
        }
    }

    int read() const {
        return HAL_GPIO_ReadPin(_pin.port, (1 << _pin.pin));
    }

    void output() {
        configure(PinDirection::Out, _mode, 0);
    }

    void input() {
        configure(PinDirection::In, _mode, 0);
    }

    void mode(PinMode mode) {
        _mode = mode;
        if (_direction == PinDirection::In) {
            configure(PinDirection::In, mode, 0);
        }
    }

    int is_connected() const {
        return (_pin.port != nullptr);
    }

    // Operator overloads
    DigitalInOut& operator= (int value) {
        write(value);
        return *this;
    }

    DigitalInOut& operator= (const DigitalInOut& rhs) {
        write(rhs.read());
        return *this;
    }

    operator int() const {
        return read();
    }

private:
    Pin _pin;
    PinDirection _direction;
    PinMode _mode;

    void configure(PinDirection dir, PinMode mode, int val) {
        _direction = dir;
        _mode = mode;

        GPIO_InitTypeDef init{};
        init.Pin = (1 << _pin.pin);
        init.Mode = (dir == PinDirection::Out) ? GPIO_MODE_OUTPUT_PP : GPIO_MODE_INPUT;
        init.Pull = static_cast<uint32_t>(mode);
        init.Speed = GPIO_SPEED_FREQ_LOW;

        HAL_GPIO_Init(_pin.port, &init);

        if (dir == PinDirection::Out) {
            write(val);
        }
    }

    void enable_gpio_clock(GPIO_TypeDef* port) {
        if (port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
        else if (port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
        else if (port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
        else if (port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
        else if (port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
        else if (port == GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE();
        // Add other ports if needed
    }
};

} // namespace sdk
