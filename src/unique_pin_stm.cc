
#include <sdk/unique_pin.h>
#include <sdk/scoped_lock.h>

namespace sdk {

void unique_pin::write(bool pin_state)
{
    HAL_GPIO_WritePin(gpio, pin, pin_state ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

void unique_pin::toggle()
{
    HAL_GPIO_TogglePin(gpio, pin);
}

bool unique_pin::read()
{
    return HAL_GPIO_ReadPin(gpio, pin) == GPIO_PIN_SET;
}

} // namespace sdk
