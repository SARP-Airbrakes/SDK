
#ifndef SDK_TIMING_H_
#define SDK_TIMING_H_

#include <sdk/vecmath.h>
#include <stm32f4xx_hal.h>

namespace sdk {

/** Time source based off of the internal SysTick. */
inline real get_tick_seconds()
{
    return (real) (HAL_GetTick() * HAL_GetTickFreq()) / 1000.0f;
}

} // namespace sdk

#endif // SDK_TIMING_H_
