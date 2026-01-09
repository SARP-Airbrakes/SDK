#pragma once
#include "stm32f4xx_hal.h"

namespace sdk {

// ===== Pin Structure =====
struct Pin {
    GPIO_TypeDef* port;
    uint16_t pin;

    constexpr Pin(GPIO_TypeDef* port, uint16_t pin) : port(port), pin(pin) {}
    constexpr Pin() : port(nullptr), pin(0xFFFF) {}
    
    constexpr bool is_valid() const { return port != nullptr; }
};

// ===== Pin Definitions =====
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

// GPIOC pins
constexpr Pin C_0  {GPIOC, 0};
constexpr Pin C_1  {GPIOC, 1};
constexpr Pin C_2  {GPIOC, 2};
constexpr Pin C_3  {GPIOC, 3};
constexpr Pin C_4  {GPIOC, 4};
constexpr Pin C_5  {GPIOC, 5};
constexpr Pin C_6  {GPIOC, 6};
constexpr Pin C_7  {GPIOC, 7};
constexpr Pin C_8  {GPIOC, 8};
constexpr Pin C_9  {GPIOC, 9};
constexpr Pin C_10 {GPIOC, 10};
constexpr Pin C_11 {GPIOC, 11};
constexpr Pin C_12 {GPIOC, 12};
constexpr Pin C_13 {GPIOC, 13};
constexpr Pin C_14 {GPIOC, 14};
constexpr Pin C_15 {GPIOC, 15};

// GPIOD pins
constexpr Pin D_0  {GPIOD, 0};
constexpr Pin D_1  {GPIOD, 1};
constexpr Pin D_2  {GPIOD, 2};
constexpr Pin D_3  {GPIOD, 3};
constexpr Pin D_4  {GPIOD, 4};
constexpr Pin D_5  {GPIOD, 5};
constexpr Pin D_6  {GPIOD, 6};
constexpr Pin D_7  {GPIOD, 7};
constexpr Pin D_8  {GPIOD, 8};
constexpr Pin D_9  {GPIOD, 9};
constexpr Pin D_10 {GPIOD, 10};
constexpr Pin D_11 {GPIOD, 11};
constexpr Pin D_12 {GPIOD, 12};
constexpr Pin D_13 {GPIOD, 13};
constexpr Pin D_14 {GPIOD, 14};
constexpr Pin D_15 {GPIOD, 15};

// GPIOE pins
constexpr Pin E_0  {GPIOE, 0};
constexpr Pin E_1  {GPIOE, 1};
constexpr Pin E_2  {GPIOE, 2};
constexpr Pin E_3  {GPIOE, 3};
constexpr Pin E_4  {GPIOE, 4};
constexpr Pin E_5  {GPIOE, 5};
constexpr Pin E_6  {GPIOE, 6};
constexpr Pin E_7  {GPIOE, 7};
constexpr Pin E_8  {GPIOE, 8};
constexpr Pin E_9  {GPIOE, 9};
constexpr Pin E_10 {GPIOE, 10};
constexpr Pin E_11 {GPIOE, 11};
constexpr Pin E_12 {GPIOE, 12};
constexpr Pin E_13 {GPIOE, 13};
constexpr Pin E_14 {GPIOE, 14};
constexpr Pin E_15 {GPIOE, 15};

// ===== Peripheral Pin Mapping Structures =====
struct I2CPinMap {
    Pin scl;
    Pin sda;
    I2C_TypeDef* instance;
    uint8_t af;
};

struct SPIPinMap {
    Pin sclk;
    Pin miso;
    Pin mosi;
    SPI_TypeDef* instance;
    uint8_t af;
};

struct UARTPinMap {
    Pin tx;
    Pin rx;
    USART_TypeDef* instance;
    uint8_t af;
};

struct PWMPinMap {
    Pin pin;
    TIM_TypeDef* instance;
    uint32_t channel;
    uint8_t af;
};

struct ADCPinMap {
    Pin pin;
    uint32_t channel;
};

// ===== I2C Pin Mapping Table =====
static const I2CPinMap I2C_PIN_MAP[] = {
    // I2C1
    {B_6,  B_7,  I2C1, GPIO_AF4_I2C1},
    {B_8,  B_9,  I2C1, GPIO_AF4_I2C1},
    
    // I2C2
    {B_10, B_11, I2C2, GPIO_AF4_I2C2},
    
    // I2C3
    {A_8,  C_9,  I2C3, GPIO_AF4_I2C3},
};

// ===== SPI Pin Mapping Table =====
static const SPIPinMap SPI_PIN_MAP[] = {
    // SPI1
    {A_5, A_6, A_7,  SPI1, GPIO_AF5_SPI1},
    {B_3, B_4, B_5,  SPI1, GPIO_AF5_SPI1},
    
    // SPI2
    {B_13, B_14, B_15, SPI2, GPIO_AF5_SPI2},
    
    // SPI3
    {C_10, C_11, C_12, SPI3, GPIO_AF6_SPI3},
    {B_3,  B_4,  B_5,  SPI3, GPIO_AF6_SPI3},
};

// ===== UART Pin Mapping Table =====
static const UARTPinMap UART_PIN_MAP[] = {
    // USART1
    {A_9,  A_10, USART1, GPIO_AF7_USART1},
    {B_6,  B_7,  USART1, GPIO_AF7_USART1},
    {A_15, B_3,  USART1, GPIO_AF7_USART1},
    
    // USART2
    {A_2,  A_3,  USART2, GPIO_AF7_USART2},
    {D_5,  D_6,  USART2, GPIO_AF7_USART2},
    
    // USART3
    {B_10, B_11, USART3, GPIO_AF7_USART3},
    {C_10, C_11, USART3, GPIO_AF7_USART3},
    {D_8,  D_9,  USART3, GPIO_AF7_USART3},
    
    // USART6
    {C_6,  C_7,  USART6, GPIO_AF8_USART6},
    {A_11, A_12, USART6, GPIO_AF8_USART6},
};

// ===== PWM Pin Mapping Table =====
static const PWMPinMap PWM_PIN_MAP[] = {
    // TIM1
    {A_8,  TIM1, TIM_CHANNEL_1, GPIO_AF1_TIM1},
    {A_9,  TIM1, TIM_CHANNEL_2, GPIO_AF1_TIM1},
    {A_10, TIM1, TIM_CHANNEL_3, GPIO_AF1_TIM1},
    {A_11, TIM1, TIM_CHANNEL_4, GPIO_AF1_TIM1},
    {E_9,  TIM1, TIM_CHANNEL_1, GPIO_AF1_TIM1},
    {E_11, TIM1, TIM_CHANNEL_2, GPIO_AF1_TIM1},
    {E_13, TIM1, TIM_CHANNEL_3, GPIO_AF1_TIM1},
    {E_14, TIM1, TIM_CHANNEL_4, GPIO_AF1_TIM1},
    
    // TIM2
    {A_0,  TIM2, TIM_CHANNEL_1, GPIO_AF1_TIM2},
    {A_1,  TIM2, TIM_CHANNEL_2, GPIO_AF1_TIM2},
    {A_2,  TIM2, TIM_CHANNEL_3, GPIO_AF1_TIM2},
    {A_3,  TIM2, TIM_CHANNEL_4, GPIO_AF1_TIM2},
    {A_15, TIM2, TIM_CHANNEL_1, GPIO_AF1_TIM2},
    {B_3,  TIM2, TIM_CHANNEL_2, GPIO_AF1_TIM2},
    {B_10, TIM2, TIM_CHANNEL_3, GPIO_AF1_TIM2},
    {B_11, TIM2, TIM_CHANNEL_4, GPIO_AF1_TIM2},
    
    // TIM3
    {A_6,  TIM3, TIM_CHANNEL_1, GPIO_AF2_TIM3},
    {A_7,  TIM3, TIM_CHANNEL_2, GPIO_AF2_TIM3},
    {B_0,  TIM3, TIM_CHANNEL_3, GPIO_AF2_TIM3},
    {B_1,  TIM3, TIM_CHANNEL_4, GPIO_AF2_TIM3},
    {B_4,  TIM3, TIM_CHANNEL_1, GPIO_AF2_TIM3},
    {B_5,  TIM3, TIM_CHANNEL_2, GPIO_AF2_TIM3},
    {C_6,  TIM3, TIM_CHANNEL_1, GPIO_AF2_TIM3},
    {C_7,  TIM3, TIM_CHANNEL_2, GPIO_AF2_TIM3},
    {C_8,  TIM3, TIM_CHANNEL_3, GPIO_AF2_TIM3},
    {C_9,  TIM3, TIM_CHANNEL_4, GPIO_AF2_TIM3},
    
    // TIM4
    {B_6,  TIM4, TIM_CHANNEL_1, GPIO_AF2_TIM4},
    {B_7,  TIM4, TIM_CHANNEL_2, GPIO_AF2_TIM4},
    {B_8,  TIM4, TIM_CHANNEL_3, GPIO_AF2_TIM4},
    {B_9,  TIM4, TIM_CHANNEL_4, GPIO_AF2_TIM4},
    {D_12, TIM4, TIM_CHANNEL_1, GPIO_AF2_TIM4},
    {D_13, TIM4, TIM_CHANNEL_2, GPIO_AF2_TIM4},
    {D_14, TIM4, TIM_CHANNEL_3, GPIO_AF2_TIM4},
    {D_15, TIM4, TIM_CHANNEL_4, GPIO_AF2_TIM4},
    
    // TIM5
    {A_0,  TIM5, TIM_CHANNEL_1, GPIO_AF2_TIM5},
    {A_1,  TIM5, TIM_CHANNEL_2, GPIO_AF2_TIM5},
    {A_2,  TIM5, TIM_CHANNEL_3, GPIO_AF2_TIM5},
    {A_3,  TIM5, TIM_CHANNEL_4, GPIO_AF2_TIM5},
};

// ===== ADC Pin Mapping Table =====
static const ADCPinMap ADC_PIN_MAP[] = {
    {A_0, ADC_CHANNEL_0},
    {A_1, ADC_CHANNEL_1},
    {A_2, ADC_CHANNEL_2},
    {A_3, ADC_CHANNEL_3},
    {A_4, ADC_CHANNEL_4},
    {A_5, ADC_CHANNEL_5},
    {A_6, ADC_CHANNEL_6},
    {A_7, ADC_CHANNEL_7},
    {B_0, ADC_CHANNEL_8},
    {B_1, ADC_CHANNEL_9},
    {C_0, ADC_CHANNEL_10},
    {C_1, ADC_CHANNEL_11},
    {C_2, ADC_CHANNEL_12},
    {C_3, ADC_CHANNEL_13},
    {C_4, ADC_CHANNEL_14},
    {C_5, ADC_CHANNEL_15},
};

// ===== Lookup Helper Functions =====
static inline const I2CPinMap* find_i2c_pinmap(Pin scl, Pin sda) {
    for (size_t i = 0; i < sizeof(I2C_PIN_MAP) / sizeof(I2CPinMap); i++) {
        if ((I2C_PIN_MAP[i].scl.port == scl.port && I2C_PIN_MAP[i].scl.pin == scl.pin) ||
            (I2C_PIN_MAP[i].sda.port == sda.port && I2C_PIN_MAP[i].sda.pin == sda.pin)) {
            return &I2C_PIN_MAP[i];
        }
    }
    return nullptr;
}

static inline const SPIPinMap* find_spi_pinmap(Pin sclk, Pin miso, Pin mosi) {
    for (size_t i = 0; i < sizeof(SPI_PIN_MAP) / sizeof(SPIPinMap); i++) {
        if ((SPI_PIN_MAP[i].sclk.port == sclk.port && SPI_PIN_MAP[i].sclk.pin == sclk.pin) ||
            (SPI_PIN_MAP[i].miso.port == miso.port && SPI_PIN_MAP[i].miso.pin == miso.pin) ||
            (SPI_PIN_MAP[i].mosi.port == mosi.port && SPI_PIN_MAP[i].mosi.pin == mosi.pin)) {
            return &SPI_PIN_MAP[i];
        }
    }
    return nullptr;
}

static inline const UARTPinMap* find_uart_pinmap(Pin tx, Pin rx) {
    for (size_t i = 0; i < sizeof(UART_PIN_MAP) / sizeof(UARTPinMap); i++) {
        if ((UART_PIN_MAP[i].tx.port == tx.port && UART_PIN_MAP[i].tx.pin == tx.pin) ||
            (UART_PIN_MAP[i].rx.port == rx.port && UART_PIN_MAP[i].rx.pin == rx.pin)) {
            return &UART_PIN_MAP[i];
        }
    }
    return nullptr;
}

static inline const PWMPinMap* find_pwm_pinmap(Pin pin) {
    for (size_t i = 0; i < sizeof(PWM_PIN_MAP) / sizeof(PWMPinMap); i++) {
        if (PWM_PIN_MAP[i].pin.port == pin.port && PWM_PIN_MAP[i].pin.pin == pin.pin) {
            return &PWM_PIN_MAP[i];
        }
    }
    return nullptr;
}

static inline const ADCPinMap* find_adc_pinmap(Pin pin) {
    for (size_t i = 0; i < sizeof(ADC_PIN_MAP) / sizeof(ADCPinMap); i++) {
        if (ADC_PIN_MAP[i].pin.port == pin.port && ADC_PIN_MAP[i].pin.pin == pin.pin) {
            return &ADC_PIN_MAP[i];
        }
    }
    return nullptr;
}

// ===== Clock Enable Helper Functions =====
static inline void enable_i2c_clock(I2C_TypeDef* instance) {
    if (instance == I2C1) __HAL_RCC_I2C1_CLK_ENABLE();
    else if (instance == I2C2) __HAL_RCC_I2C2_CLK_ENABLE();
    else if (instance == I2C3) __HAL_RCC_I2C3_CLK_ENABLE();
}

static inline void enable_spi_clock(SPI_TypeDef* instance) {
    if (instance == SPI1) __HAL_RCC_SPI1_CLK_ENABLE();
    else if (instance == SPI2) __HAL_RCC_SPI2_CLK_ENABLE();
    else if (instance == SPI3) __HAL_RCC_SPI3_CLK_ENABLE();
    else if (instance == SPI4) __HAL_RCC_SPI4_CLK_ENABLE();
}

static inline void enable_uart_clock(USART_TypeDef* instance) {
    if (instance == USART1) __HAL_RCC_USART1_CLK_ENABLE();
    else if (instance == USART2) __HAL_RCC_USART2_CLK_ENABLE();
    else if (instance == USART3) __HAL_RCC_USART3_CLK_ENABLE();
    else if (instance == UART4) __HAL_RCC_UART4_CLK_ENABLE();
    else if (instance == UART5) __HAL_RCC_UART5_CLK_ENABLE();
    else if (instance == USART6) __HAL_RCC_USART6_CLK_ENABLE();
}

static inline void enable_tim_clock(TIM_TypeDef* instance) {
    if (instance == TIM1) __HAL_RCC_TIM1_CLK_ENABLE();
    else if (instance == TIM2) __HAL_RCC_TIM2_CLK_ENABLE();
    else if (instance == TIM3) __HAL_RCC_TIM3_CLK_ENABLE();
    else if (instance == TIM4) __HAL_RCC_TIM4_CLK_ENABLE();
    else if (instance == TIM5) __HAL_RCC_TIM5_CLK_ENABLE();
    else if (instance == TIM6) __HAL_RCC_TIM6_CLK_ENABLE();
    else if (instance == TIM7) __HAL_RCC_TIM7_CLK_ENABLE();
    else if (instance == TIM8) __HAL_RCC_TIM8_CLK_ENABLE();
    else if (instance == TIM9) __HAL_RCC_TIM9_CLK_ENABLE();
    else if (instance == TIM10) __HAL_RCC_TIM10_CLK_ENABLE();
    else if (instance == TIM11) __HAL_RCC_TIM11_CLK_ENABLE();
    else if (instance == TIM12) __HAL_RCC_TIM12_CLK_ENABLE();
    else if (instance == TIM13) __HAL_RCC_TIM13_CLK_ENABLE();
    else if (instance == TIM14) __HAL_RCC_TIM14_CLK_ENABLE();
}

} // namespace sdk
