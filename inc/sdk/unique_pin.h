
#ifndef AIRBRAKES_SDK_UNIQUE_PIN_H_
#define AIRBRAKES_SDK_UNIQUE_PIN_H_

#include <stm32f4xx.h>

namespace sdk {

/**
 * Represents an interface for one GPIO pin that should only be accessible from
 * one thread.
 */
class unique_pin {
public:

    explicit unique_pin(GPIO_TypeDef *gpio, uint16_t pin) : gpio(gpio), pin(pin)
    {
    }

    // move-only semantics
    unique_pin(const unique_pin &) = delete;
    unique_pin(unique_pin &&) = default;
    unique_pin &operator=(const unique_pin &) = delete;
    unique_pin &operator=(unique_pin &&) = default;

    void write(bool pin_state);
    void toggle();
    bool read();

private:
    GPIO_TypeDef *gpio;
    uint16_t pin;

};

}

#endif // AIRBRAKES_SDK_SHARED_PIN_H_
