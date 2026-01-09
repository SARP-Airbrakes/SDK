#pragma once
#include "base.hpp"
#include "pinmap.hpp"

namespace sdk {

class PWM {
public:
    PWM(Pin pin, uint32_t frequency = 1000, float duty_cycle = 0.0f) {
        _pin = pin;
        
        const PWMPinMap* pinmap = find_pwm_pinmap(pin);
        SDK_ASSERT(pinmap != nullptr);
        
        _htim.Instance = pinmap->instance;
        _channel = pinmap->channel;
        _af = pinmap->af;
        
        enable_gpio_clock(_pin.port);
        enable_tim_clock(_htim.Instance);
        
        configure_pin();
        configure_timer(frequency);
        write(duty_cycle);
    }

    ~PWM() {
        HAL_TIM_PWM_Stop(&_htim, _channel);
        HAL_TIM_PWM_DeInit(&_htim);
    }

    void write(float value) {
        _duty_cycle = clamp(value, 0.0f, 1.0f);
        uint32_t pulse = static_cast<uint32_t>(_duty_cycle * (_htim.Init.Period + 1));
        __HAL_TIM_SET_COMPARE(&_htim, _channel, pulse);
    }

    float read() const {
        return _duty_cycle;
    }

    void period(float seconds) {
        frequency(1.0f / seconds);
    }

    void period_ms(uint32_t ms) {
        frequency(1000.0f / ms);
    }

    void period_us(uint32_t us) {
        frequency(1000000.0f / us);
    }

    void frequency(float hz) {
        HAL_TIM_PWM_Stop(&_htim, _channel);
        configure_timer(static_cast<uint32_t>(hz));
        write(_duty_cycle);
    }

    void pulse_width(float seconds) {
        float period_sec = 1.0f / _frequency;
        write(seconds / period_sec);
    }

    void pulse_width_ms(uint32_t ms) {
        pulse_width(ms / 1000.0f);
    }

    void pulse_width_us(uint32_t us) {
        pulse_width(us / 1000000.0f);
    }

    PWM& operator=(float value) {
        write(value);
        return *this;
    }

    operator float() const {
        return read();
    }

private:
    Pin _pin;
    TIM_HandleTypeDef _htim{};
    uint32_t _channel = 0;
    uint8_t _af = 0;
    float _duty_cycle = 0.0f;
    float _frequency = 1000.0f;

    void configure_pin() {
        GPIO_InitTypeDef init{};
        init.Pin = (1 << _pin.pin);
        init.Mode = GPIO_MODE_AF_PP;
        init.Pull = GPIO_NOPULL;
        init.Speed = GPIO_SPEED_FREQ_HIGH;
        init.Alternate = _af;
        HAL_GPIO_Init(_pin.port, &init);
    }

    void configure_timer(uint32_t frequency) {
        _frequency = static_cast<float>(frequency);
        
        uint32_t timer_clock = get_apb_clock(_htim.Instance) * 2; // Timer clock is 2x APB clock
        uint32_t prescaler = 0;
        uint32_t period = 0;
        
        // Calculate prescaler and period
        // frequency = timer_clock / ((prescaler + 1) * (period + 1))
        if (frequency > 0) {
            uint32_t divider = timer_clock / frequency;
            
            // Try to maximize period for better resolution
            if (divider <= 65536) {
                prescaler = 0;
                period = divider - 1;
            } else {
                prescaler = (divider / 65536);
                period = (divider / (prescaler + 1)) - 1;
            }
            
            period = clamp(period, 1U, 65535U);
            prescaler = clamp(prescaler, 0U, 65535U);
        } else {
            prescaler = 65535;
            period = 65535;
        }
        
        _htim.Init.Prescaler = prescaler;
        _htim.Init.CounterMode = TIM_COUNTERMODE_UP;
        _htim.Init.Period = period;
        _htim.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
        _htim.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_ENABLE;
        
        HAL_StatusTypeDef status = HAL_TIM_PWM_Init(&_htim);
        SDK_ASSERT(status == HAL_OK);
        
        TIM_OC_InitTypeDef config{};
        config.OCMode = TIM_OCMODE_PWM1;
        config.Pulse = 0;
        config.OCPolarity = TIM_OCPOLARITY_HIGH;
        config.OCFastMode = TIM_OCFAST_DISABLE;
        
        status = HAL_TIM_PWM_ConfigChannel(&_htim, &config, _channel);
        SDK_ASSERT(status == HAL_OK);
        
        status = HAL_TIM_PWM_Start(&_htim, _channel);
        SDK_ASSERT(status == HAL_OK);
    }
};

} // namespace sdk
