
#include "stm32f401xc.h"
#include <sdk/pwm.h>

namespace sdk {

void pwm::start()
{
    HAL_TIM_PWM_Start(htim, (uint32_t) channel);
}

void pwm::stop()
{
    HAL_TIM_PWM_Stop(htim, (uint32_t) channel);
}

void pwm::set(real value)
{
    // clamp value between [0,1]
    if (value < 0) value = 0;
    else if (value > 1) value = 1;

    uint32_t ccr = (uint32_t)(value / (real) htim->Instance->ARR);
    switch (channel) {
    case tim_channel::CHANNEL_1:
        htim->Instance->CCR1 = ccr;
        break;
    case tim_channel::CHANNEL_2:
        htim->Instance->CCR2 = ccr;
        break;
    case tim_channel::CHANNEL_3:
        htim->Instance->CCR3 = ccr;
        break;
    case tim_channel::CHANNEL_4:
        htim->Instance->CCR4 = ccr;
        break;
    }

}

pwm::real pwm::get_resolution()
{
    return 1.0f / (real) htim->Instance->ARR;
}

} // namespace sdk
