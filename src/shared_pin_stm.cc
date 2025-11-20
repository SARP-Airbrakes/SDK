
#include <sdk/shared_pin.h>

#include <sdk/scoped_lock.h>

namespace sdk {

void shared_pin::write(bool pin_state)
{
    scoped_lock lock(pin_mutex);
    HAL_GPIO_WritePin(gpio, pin, pin_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void shared_pin::toggle()
{
    scoped_lock lock(pin_mutex);
    HAL_GPIO_TogglePin(gpio, pin);
}

bool shared_pin::read()
{
    scoped_lock lock(pin_mutex);
    return HAL_GPIO_ReadPin(gpio, pin) == GPIO_PIN_SET;
}

} // namespace sdk
