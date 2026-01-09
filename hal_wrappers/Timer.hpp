#pragma once
#include "base.hpp"

namespace sdk {

class Timer {
public:
    using Callback = void (*)(void);
    
    Timer() {
        _tim_instance = find_available_timer();
        SDK_ASSERT(_tim_instance != nullptr);
        
        enable_tim_clock(_tim_instance);
        _callback = nullptr;
    }
    
    ~Timer() {
        stop();
        HAL_TIM_Base_DeInit(&_htim);
    }
    
    void start() {
        HAL_TIM_Base_Start_IT(&_htim);
    }
    
    void stop() {
        HAL_TIM_Base_Stop_IT(&_htim);
    }
    
    void reset() {
        __HAL_TIM_SET_COUNTER(&_htim, 0);
    }
    
    uint32_t read_us() {
        // Assumes timer is configured for microsecond resolution
        return __HAL_TIM_GET_COUNTER(&_htim);
    }
    
    uint32_t read_ms() {
        return read_us() / 1000;
    }
    
    float read() {
        return read_us() / 1000000.0f;
    }
    
    void attach(Callback func, float interval_sec) {
        _callback = func;
        configure_timer(static_cast<uint32_t>(interval_sec * 1000000));
        start();
    }
    
    void attach_us(Callback func, uint32_t interval_us) {
        _callback = func;
        configure_timer(interval_us);
        start();
    }
    
    void detach() {
        stop();
        _callback = nullptr;
    }
    
    void handle_interrupt() {
        if (_callback) {
            _callback();
        }
    }
    
    TIM_HandleTypeDef* get_handle() { return &_htim; }

private:
    TIM_HandleTypeDef _htim{};
    TIM_TypeDef* _tim_instance = nullptr;
    Callback _callback = nullptr;
    
    TIM_TypeDef* find_available_timer() {
        // Use TIM6 or TIM7 (basic timers) for general purpose
        // These are typically not used by PWM/other features
        static bool tim6_used = false;
        static bool tim7_used = false;
        
        if (!tim6_used) {
            tim6_used = true;
            return TIM6;
        }
        if (!tim7_used) {
            tim7_used = true;
            return TIM7;
        }
        
        // Fallback to other timers
        return TIM6;
    }
    
    void configure_timer(uint32_t period_us) {
        uint32_t timer_clock = get_apb_clock(_tim_instance) * 2;
        
        // Configure for microsecond resolution
        uint32_t prescaler = (timer_clock / 1000000) - 1; // 1 MHz = 1 us per tick
        uint32_t period = period_us - 1;
        
        _htim.Instance = _tim_instance;
        _htim.Init.Prescaler = prescaler;
        _htim.Init.CounterMode = TIM_COUNTERMODE_UP;
        _htim.Init.Period = period;
        _htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        _htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
        
        HAL_StatusTypeDef status = HAL_TIM_Base_Init(&_htim);
        SDK_ASSERT(status == HAL_OK);
        
        // Enable interrupt
        HAL_NVIC_SetPriority(get_irq_number(), 6, 0);
        HAL_NVIC_EnableIRQ(get_irq_number());
    }
    
    IRQn_Type get_irq_number() {
        if (_tim_instance == TIM1) return TIM1_UP_TIM10_IRQn;
        if (_tim_instance == TIM2) return TIM2_IRQn;
        if (_tim_instance == TIM3) return TIM3_IRQn;
        if (_tim_instance == TIM4) return TIM4_IRQn;
        if (_tim_instance == TIM5) return TIM5_IRQn;
        if (_tim_instance == TIM6) return TIM6_DAC_IRQn;
        if (_tim_instance == TIM7) return TIM7_IRQn;
        return TIM2_IRQn;
    }
};

class Ticker {
public:
    using Callback = void (*)(void);
    
    void attach(Callback func, float interval_sec) {
        _timer.attach(func, interval_sec);
    }
    
    void attach_us(Callback func, uint32_t interval_us) {
        _timer.attach_us(func, interval_us);
    }
    
    void detach() {
        _timer.detach();
    }

private:
    Timer _timer;
};

class Timeout {
public:
    using Callback = void (*)(void);
    
    void attach(Callback func, float delay_sec) {
        _callback = func;
        _triggered = false;
        _timer.attach([this]() { this->trigger(); }, delay_sec);
    }
    
    void attach_us(Callback func, uint32_t delay_us) {
        _callback = func;
        _triggered = false;
        _timer.attach_us([this]() { this->trigger(); }, delay_us);
    }
    
    void detach() {
        _timer.detach();
        _triggered = false;
    }

private:
    Timer _timer;
    Callback _callback = nullptr;
    bool _triggered = false;
    
    void trigger() {
        if (!_triggered && _callback) {
            _triggered = true;
            _timer.stop();
            _callback();
        }
    }
};

// Simple delay functions using busy-wait
class Wait {
public:
    static void ms(uint32_t ms) {
        HAL_Delay(ms);
    }
    
    static void us(uint32_t us) {
        uint32_t start = DWT->CYCCNT;
        uint32_t cycles = (SystemCoreClock / 1000000) * us;
        while ((DWT->CYCCNT - start) < cycles);
    }
    
    static void s(float seconds) {
        ms(static_cast<uint32_t>(seconds * 1000));
    }
};

} // namespace sdk
