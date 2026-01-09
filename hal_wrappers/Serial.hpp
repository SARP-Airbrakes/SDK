#pragma once
#include "base.hpp"
#include "pinmap.hpp"

namespace sdk {

class Serial {
public:
    enum Parity {
        NONE = 0,
        ODD = 1,
        EVEN = 2
    };

    enum StopBits {
        STOP_1 = 0,
        STOP_2 = 1
    };

    Serial(Pin tx, Pin rx, uint32_t baud_rate = 9600, 
           Parity parity = NONE, StopBits stop_bits = STOP_1) {
        _tx = tx;
        _rx = rx;
        
        const UARTPinMap* pinmap = find_uart_pinmap(tx, rx);
        SDK_ASSERT(pinmap != nullptr);
        
        _uart_instance = pinmap->instance;
        _af = pinmap->af;
        
        enable_gpio_clock(_tx.port);
        enable_gpio_clock(_rx.port);
        enable_uart_clock(_uart_instance);
        
        configure_pins();
        configure_uart(baud_rate, parity, stop_bits);
    }

    ~Serial() {
        HAL_UART_DeInit(&_huart);
    }

    bool write(const uint8_t* data, uint16_t length, uint32_t timeout = 1000) {
        return HAL_UART_Transmit(&_huart, const_cast<uint8_t*>(data), length, timeout) == HAL_OK;
    }

    bool write(const char* str, uint32_t timeout = 1000) {
        uint16_t len = 0;
        while (str[len]) len++;
        return write(reinterpret_cast<const uint8_t*>(str), len, timeout);
    }

    bool putc(char c, uint32_t timeout = 1000) {
        return HAL_UART_Transmit(&_huart, reinterpret_cast<uint8_t*>(&c), 1, timeout) == HAL_OK;
    }

    bool read(uint8_t* data, uint16_t length, uint32_t timeout = 1000) {
        return HAL_UART_Receive(&_huart, data, length, timeout) == HAL_OK;
    }

    bool getc(char* c, uint32_t timeout = 1000) {
        return HAL_UART_Receive(&_huart, reinterpret_cast<uint8_t*>(c), 1, timeout) == HAL_OK;
    }

    bool readable() const {
        return __HAL_UART_GET_FLAG(&_huart, UART_FLAG_RXNE);
    }

    bool writable() const {
        return __HAL_UART_GET_FLAG(&_huart, UART_FLAG_TXE);
    }

    void baud_rate(uint32_t baud) {
        _huart.Init.BaudRate = baud;
        HAL_UART_Init(&_huart);
    }

    UART_HandleTypeDef* get_handle() { return &_huart; }

private:
    Pin _tx;
    Pin _rx;
    UART_HandleTypeDef _huart{};
    USART_TypeDef* _uart_instance = nullptr;
    uint8_t _af = 0;

    void configure_pins() {
        GPIO_InitTypeDef init{};
        init.Mode = GPIO_MODE_AF_PP;
        init.Pull = GPIO_PULLUP;
        init.Speed = GPIO_SPEED_FREQ_HIGH;
        init.Alternate = _af;

        init.Pin = (1 << _tx.pin);
        HAL_GPIO_Init(_tx.port, &init);

        init.Pin = (1 << _rx.pin);
        HAL_GPIO_Init(_rx.port, &init);
    }

    void configure_uart(uint32_t baud_rate, Parity parity, StopBits stop_bits) {
        _huart.Instance = _uart_instance;
        _huart.Init.BaudRate = baud_rate;
        _huart.Init.WordLength = (parity == NONE) ? UART_WORDLENGTH_8B : UART_WORDLENGTH_9B;
        _huart.Init.StopBits = (stop_bits == STOP_1) ? UART_STOPBITS_1 : UART_STOPBITS_2;
        
        switch (parity) {
            case NONE:
                _huart.Init.Parity = UART_PARITY_NONE;
                break;
            case ODD:
                _huart.Init.Parity = UART_PARITY_ODD;
                break;
            case EVEN:
                _huart.Init.Parity = UART_PARITY_EVEN;
                break;
        }
        
        _huart.Init.Mode = UART_MODE_TX_RX;
        _huart.Init.HwFlowCtl = UART_HWCONTROL_NONE;
        _huart.Init.OverSampling = UART_OVERSAMPLING_16;
        
        HAL_StatusTypeDef status = HAL_UART_Init(&_huart);
        SDK_ASSERT(status == HAL_OK);
    }
};

} // namespace sdk
