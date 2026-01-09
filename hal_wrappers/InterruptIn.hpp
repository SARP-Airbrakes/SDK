#pragma once
#include "base.hpp"
#include "pinmap.hpp"

namespace sdk {

class InterruptIn {
public:
    enum Edge {
        RISING  = 0,
        FALLING = 1,
        BOTH    = 2
    };
    
    using Callback = void (*)(void);
    
    InterruptIn(Pin pin, Edge edge = RISING) {
        _pin = pin;
        _edge = edge;
        
        enable_gpio_clock(_pin.port);
        configure_pin();
        configure_interrupt(edge);
        
        register_pin(this);
    }
    
    ~InterruptIn() {
        disable();
        unregister_pin(this);
    }
    
    void rise(Callback func) {
        _rise_callback = func;
        if (_edge == FALLING) {
            set_edge(BOTH);
        } else if (_edge != BOTH) {
            set_edge(RISING);
        }
    }
    
    void fall(Callback func) {
        _fall_callback = func;
        if (_edge == RISING) {
            set_edge(BOTH);
        } else if (_edge != BOTH) {
            set_edge(FALLING);
        }
    }
    
    void mode(GPIOPuPd_TypeDef pull) {
        GPIO_InitTypeDef init{};
        init.Pin = (1 << _pin.pin);
        init.Mode = get_gpio_mode(_edge);
        init.Pull = pull;
        HAL_GPIO_Init(_pin.port, &init);
    }
    
    void enable() {
        HAL_NVIC_EnableIRQ(get_irq_number());
    }
    
    void disable() {
        HAL_NVIC_DisableIRQ(get_irq_number());
    }
    
    int read() const {
        return HAL_GPIO_ReadPin(_pin.port, (1 << _pin.pin));
    }
    
    operator int() const {
        return read();
    }
    
    void handle_interrupt() {
        if (_edge == RISING || _edge == BOTH) {
            if (read() && _rise_callback) {
                _rise_callback();
            }
        }
        if (_edge == FALLING || _edge == BOTH) {
            if (!read() && _fall_callback) {
                _fall_callback();
            }
        }
    }

private:
    Pin _pin;
    Edge _edge;
    Callback _rise_callback = nullptr;
    Callback _fall_callback = nullptr;
    
    void configure_pin() {
        GPIO_InitTypeDef init{};
        init.Pin = (1 << _pin.pin);
        init.Mode = get_gpio_mode(_edge);
        init.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(_pin.port, &init);
    }
    
    void configure_interrupt(Edge edge) {
        uint32_t exti_line = (1 << _pin.pin);
        
        // Enable SYSCFG clock
        __HAL_RCC_SYSCFG_CLK_ENABLE();
        
        // Configure EXTI line
        uint32_t port_source = get_port_source();
        uint32_t pin_source = _pin.pin;
        
        // Map GPIO to EXTI
        SYSCFG->EXTICR[pin_source / 4] &= ~(0x0F << ((pin_source % 4) * 4));
        SYSCFG->EXTICR[pin_source / 4] |= (port_source << ((pin_source % 4) * 4));
        
        // Enable interrupt
        HAL_NVIC_SetPriority(get_irq_number(), 5, 0);
        HAL_NVIC_EnableIRQ(get_irq_number());
    }
    
    void set_edge(Edge new_edge) {
        _edge = new_edge;
        configure_pin();
    }
    
    uint32_t get_gpio_mode(Edge edge) {
        switch (edge) {
            case RISING:  return GPIO_MODE_IT_RISING;
            case FALLING: return GPIO_MODE_IT_FALLING;
            case BOTH:    return GPIO_MODE_IT_RISING_FALLING;
            default:      return GPIO_MODE_IT_RISING;
        }
    }
    
    uint32_t get_port_source() {
        if (_pin.port == GPIOA) return 0;
        if (_pin.port == GPIOB) return 1;
        if (_pin.port == GPIOC) return 2;
        if (_pin.port == GPIOD) return 3;
        if (_pin.port == GPIOE) return 4;
        if (_pin.port == GPIOF) return 5;
        if (_pin.port == GPIOG) return 6;
        if (_pin.port == GPIOH) return 7;
        return 0;
    }
    
    IRQn_Type get_irq_number() {
        if (_pin.pin == 0)  return EXTI0_IRQn;
        if (_pin.pin == 1)  return EXTI1_IRQn;
        if (_pin.pin == 2)  return EXTI2_IRQn;
        if (_pin.pin == 3)  return EXTI3_IRQn;
        if (_pin.pin == 4)  return EXTI4_IRQn;
        if (_pin.pin >= 5 && _pin.pin <= 9)  return EXTI9_5_IRQn;
        if (_pin.pin >= 10 && _pin.pin <= 15) return EXTI15_10_IRQn;
        return EXTI0_IRQn;
    }
    
    // Static registry for interrupt handling
    static InterruptIn* _instances[16];
    
    static void register_pin(InterruptIn* instance) {
        _instances[instance->_pin.pin] = instance;
    }
    
    static void unregister_pin(InterruptIn* instance) {
        _instances[instance->_pin.pin] = nullptr;
    }
    
    friend void ::EXTI0_IRQHandler(void);
    friend void ::EXTI1_IRQHandler(void);
    friend void ::EXTI2_IRQHandler(void);
    friend void ::EXTI3_IRQHandler(void);
    friend void ::EXTI4_IRQHandler(void);
    friend void ::EXTI9_5_IRQHandler(void);
    friend void ::EXTI15_10_IRQHandler(void);
    
public:
    static void handle_exti_irq(uint16_t pin) {
        if (_instances[pin]) {
            _instances[pin]->handle_interrupt();
        }
    }
};

// Static member initialization
InterruptIn* InterruptIn::_instances[16] = {nullptr};

} // namespace sdk

// IRQ Handlers - Place in your main.cpp or separate irq_handlers.cpp
extern "C" {

void EXTI0_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_0)) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_0);
        sdk::InterruptIn::handle_exti_irq(0);
    }
}

void EXTI1_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_1)) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_1);
        sdk::InterruptIn::handle_exti_irq(1);
    }
}

void EXTI2_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_2)) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_2);
        sdk::InterruptIn::handle_exti_irq(2);
    }
}

void EXTI3_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_3)) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_3);
        sdk::InterruptIn::handle_exti_irq(3);
    }
}

void EXTI4_IRQHandler(void) {
    if (__HAL_GPIO_EXTI_GET_IT(GPIO_PIN_4)) {
        __HAL_GPIO_EXTI_CLEAR_IT(GPIO_PIN_4);
        sdk::InterruptIn::handle_exti_irq(4);
    }
}

void EXTI9_5_IRQHandler(void) {
    for (uint16_t pin = 5; pin <= 9; pin++) {
        if (__HAL_GPIO_EXTI_GET_IT(1 << pin)) {
            __HAL_GPIO_EXTI_CLEAR_IT(1 << pin);
            sdk::InterruptIn::handle_exti_irq(pin);
        }
    }
}

void EXTI15_10_IRQHandler(void) {
    for (uint16_t pin = 10; pin <= 15; pin++) {
        if (__HAL_GPIO_EXTI_GET_IT(1 << pin)) {
            __HAL_GPIO_EXTI_CLEAR_IT(1 << pin);
            sdk::InterruptIn::handle_exti_irq(pin);
        }
    }
}

} // extern "C"
