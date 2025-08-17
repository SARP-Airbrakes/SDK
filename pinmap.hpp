#pragma once
#include "stm32f4xx_hal.h"

namespace sdk {

struct Pin {
    GPIO_TypeDef* port;
    uint16_t pin;

    constexpr Pin(GPIO_TypeDef* port, uint16_t pin) : port(port), pin(pin) {}

    // Default contructor
    constexpr Pin() : port(nullptr), pin(0xFFFF) {}
};

// GPIOA pins
constexpr Pin A_0  {GPIOA, 0};
constexpr Pin A_1  {GPIOA, 1};
constexpr Pin A_2  {GPIOA, 2};
constexpr Pin A_3  {GPIOA, 3};
constexpr Pin A_4  {GPIOA, 4};
constexpr Pin A_5  {GPIOA, 5};
constexpr Pin A_6  {GPIOA, 6};
constexpr Pin A_7  {GPIOA, 7};
constexpr Pin A_8  {GPIOA, 8};
constexpr Pin A_9  {GPIOA, 9};
constexpr Pin A_10 {GPIOA, 10};
constexpr Pin A_11 {GPIOA, 11};
constexpr Pin A_12 {GPIOA, 12};
constexpr Pin A_13 {GPIOA, 13};
constexpr Pin A_14 {GPIOA, 14};
constexpr Pin A_15 {GPIOA, 15};

// GPIOB pins
constexpr Pin B_0  {GPIOB, 0};
constexpr Pin B_1  {GPIOB, 1};
constexpr Pin B_2  {GPIOB, 2};
constexpr Pin B_3  {GPIOB, 3};
constexpr Pin B_4  {GPIOB, 4};
constexpr Pin B_5  {GPIOB, 5};
constexpr Pin B_6  {GPIOB, 6};
constexpr Pin B_7  {GPIOB, 7};
constexpr Pin B_8  {GPIOB, 8};
constexpr Pin B_9  {GPIOB, 9};
constexpr Pin B_10 {GPIOB, 10};
constexpr Pin B_11 {GPIOB, 11};
constexpr Pin B_12 {GPIOB, 12};
constexpr Pin B_13 {GPIOB, 13};
constexpr Pin B_14 {GPIOB, 14};
constexpr Pin B_15 {GPIOB, 15};

// GPIOC pins (limited on STM32F401CCU)
constexpr Pin C_13 {GPIOC, 13};
constexpr Pin C_14 {GPIOC, 14};
constexpr Pin C_15 {GPIOC, 15};

} // namespace sdk
