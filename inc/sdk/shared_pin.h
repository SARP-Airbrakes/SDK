
#ifndef AIRBRAKES_SDK_SHARED_PIN_H_
#define AIRBRAKES_SDK_SHARED_PIN_H_

#include <sdk/mutex.h>
#include <stm32f4xx.h>

namespace sdk {

/**
 * Represents a thread-safe interface for one GPIO pin.
 */
class shared_pin {
public:

    explicit shared_pin(GPIO_TypeDef *gpio, uint16_t pin) : gpio(gpio), pin(pin)
    {
    }

    void write(bool pin_state);
    void toggle();
    bool read();

private:
    GPIO_TypeDef *gpio;
    uint16_t pin;

    mutex pin_mutex;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_SHARED_PIN_H_
