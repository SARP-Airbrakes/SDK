#pragma once
/**
 * @file base.hpp
 * @brief Core SDK base include for STM32F4 family.
 *
 * Provides common includes, utilities, and namespace setup
 * for all SDK modules (bus wrappers, devices, etc.).
 */

extern "C" {
#include "stm32f4xx_hal.h"
}

#include <cstdint>
#include <cstddef>

namespace sdk {

// ===== Common Macros =====
#ifndef SDK_ASSERT
#define SDK_ASSERT(cond) do { if(!(cond)) { __disable_irq(); while(1){} } } while(0)
#endif

// ===== Utility Functions =====
template<typename T>
static inline T clamp(T value, T min_val, T max_val) {
    return (value < min_val) ? min_val :
           (value > max_val) ? max_val :
           value;
}

} // namespace sdk