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

// ===== GPIO Clock Enable =====
static inline void enable_gpio_clock(GPIO_TypeDef* port) {
    if (port == GPIOA) __HAL_RCC_GPIOA_CLK_ENABLE();
    else if (port == GPIOB) __HAL_RCC_GPIOB_CLK_ENABLE();
    else if (port == GPIOC) __HAL_RCC_GPIOC_CLK_ENABLE();
    else if (port == GPIOD) __HAL_RCC_GPIOD_CLK_ENABLE();
    else if (port == GPIOE) __HAL_RCC_GPIOE_CLK_ENABLE();
    else if (port == GPIOF) __HAL_RCC_GPIOF_CLK_ENABLE();
    else if (port == GPIOG) __HAL_RCC_GPIOG_CLK_ENABLE();
    else if (port == GPIOH) __HAL_RCC_GPIOH_CLK_ENABLE();
}

// ===== Peripheral Instance Getters =====
static inline uint32_t get_apb_clock(void* instance) {
    // Check if peripheral is on APB1 or APB2
    if (instance == TIM2 || instance == TIM3 || instance == TIM4 || instance == TIM5 ||
        instance == TIM6 || instance == TIM7 || instance == TIM12 || instance == TIM13 || instance == TIM14 ||
        instance == I2C1 || instance == I2C2 || instance == I2C3 ||
        instance == SPI2 || instance == SPI3 ||
        instance == USART2 || instance == USART3 || instance == UART4 || instance == UART5) {
        return HAL_RCC_GetPCLK1Freq();
    }
    // APB2 peripherals
    return HAL_RCC_GetPCLK2Freq();
}

} // namespace sdk