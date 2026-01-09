#pragma once
#include "base.hpp"
#include "pinmap.hpp"

namespace sdk {

class AnalogIn {
public:
    explicit AnalogIn(Pin pin) {
        _pin = pin;
        
        const ADCPinMap* pinmap = find_adc_pinmap(pin);
        SDK_ASSERT(pinmap != nullptr);
        
        _channel = pinmap->channel;
        
        enable_gpio_clock(_pin.port);
        __HAL_RCC_ADC1_CLK_ENABLE();
        
        configure_pin();
        configure_adc();
    }

    ~AnalogIn() {
        HAL_ADC_DeInit(&_hadc);
    }

    uint16_t read_u16() {
        HAL_ADC_Start(&_hadc);
        if (HAL_ADC_PollForConversion(&_hadc, 100) == HAL_OK) {
            uint32_t value = HAL_ADC_GetValue(&_hadc);
            // Convert 12-bit ADC value to 16-bit
            return static_cast<uint16_t>((value * 65535) / 4095);
        }
        return 0;
    }

    float read() {
        return read_u16() / 65535.0f;
    }

    uint16_t read_raw() {
        HAL_ADC_Start(&_hadc);
        if (HAL_ADC_PollForConversion(&_hadc, 100) == HAL_OK) {
            return static_cast<uint16_t>(HAL_ADC_GetValue(&_hadc));
        }
        return 0;
    }

    float read_voltage(float vref = 3.3f) {
        return read() * vref;
    }

    operator float() const {
        return const_cast<AnalogIn*>(this)->read();
    }

    operator uint16_t() const {
        return const_cast<AnalogIn*>(this)->read_u16();
    }

private:
    Pin _pin;
    ADC_HandleTypeDef _hadc{};
    uint32_t _channel = 0;

    void configure_pin() {
        GPIO_InitTypeDef init{};
        init.Pin = (1 << _pin.pin);
        init.Mode = GPIO_MODE_ANALOG;
        init.Pull = GPIO_NOPULL;
        HAL_GPIO_Init(_pin.port, &init);
    }

    void configure_adc() {
        
        _hadc.Instance = ADC1;
        _hadc.Init.ClockPrescaler = ADC_CLOCK_SYNC_PCLK_DIV4;
        _hadc.Init.Resolution = ADC_RESOLUTION_12B;
        _hadc.Init.ScanConvMode = DISABLE;
        _hadc.Init.ContinuousConvMode = DISABLE;
        _hadc.Init.DiscontinuousConvMode = DISABLE;
        _hadc.Init.ExternalTrigConvEdge = ADC_EXTERNALTRIGCONVEDGE_NONE;
        _hadc.Init.ExternalTrigConv = ADC_SOFTWARE_START;
        _hadc.Init.DataAlign = ADC_DATAALIGN_RIGHT;
        _hadc.Init.NbrOfConversion = 1;
        _hadc.Init.DMAContinuousRequests = DISABLE;
        _hadc.Init.EOCSelection = ADC_EOC_SINGLE_CONV;
        
        HAL_StatusTypeDef status = HAL_ADC_Init(&_hadc);
        SDK_ASSERT(status == HAL_OK);
        
        ADC_ChannelConfTypeDef config{};
        config.Channel = _channel;
        config.Rank = 1;
        config.SamplingTime = ADC_SAMPLETIME_84CYCLES;
        
        status = HAL_ADC_ConfigChannel(&_hadc, &config);
        SDK_ASSERT(status == HAL_OK);
    }
};

} // namespace sdk
