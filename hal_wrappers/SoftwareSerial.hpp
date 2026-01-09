#pragma once
#include "base.hpp"
#include "pinmap.hpp"
#include "Timer.hpp"

namespace sdk {

class SoftwareSerial {
public:
    SoftwareSerial(Pin tx, Pin rx, uint32_t baud_rate = 9600) 
        : _tx(tx, 1), _rx(rx, GPIO_PULLUP), _baud_rate(baud_rate) {
        
        _bit_period_us = 1000000 / baud_rate;
        _rx_callback = nullptr;
    }
    
    bool putc(char c) {
        // Start bit (LOW)
        _tx = 0;
        Wait::us(_bit_period_us);
        
        // Data bits (LSB first)
        for (int i = 0; i < 8; i++) {
            _tx = (c >> i) & 0x01;
            Wait::us(_bit_period_us);
        }
        
        // Stop bit (HIGH)
        _tx = 1;
        Wait::us(_bit_period_us);
        
        return true;
    }
    
    bool write(const char* str) {
        while (*str) {
            if (!putc(*str++)) return false;
        }
        return true;
    }
    
    bool write(const uint8_t* data, uint16_t length) {
        for (uint16_t i = 0; i < length; i++) {
            if (!putc(data[i])) return false;
        }
        return true;
    }
    
    bool readable() const {
        return _rx == 0; // Start bit detected
    }
    
    bool getc(char* c) {
        // Wait for start bit
        while (_rx == 1);
        
        // Wait for middle of start bit
        Wait::us(_bit_period_us / 2);
        
        // Verify start bit
        if (_rx == 1) return false; // False start
        
        // Wait for middle of first data bit
        Wait::us(_bit_period_us);
        
        // Read data bits (LSB first)
        uint8_t data = 0;
        for (int i = 0; i < 8; i++) {
            if (_rx == 1) {
                data |= (1 << i);
            }
            Wait::us(_bit_period_us);
        }
        
        // Read stop bit (should be HIGH)
        if (_rx == 0) return false; // Framing error
        
        *c = data;
        return true;
    }
    
    void attach(void (*func)(void)) {
        _rx_callback = func;
        _rx_interrupt.rise(_rx_callback);
    }
    
    void baud_rate(uint32_t baud) {
        _baud_rate = baud;
        _bit_period_us = 1000000 / baud;
    }

private:
    DigitalOut _tx;
    DigitalIn _rx;
    InterruptIn _rx_interrupt;
    uint32_t _baud_rate;
    uint32_t _bit_period_us;
    void (*_rx_callback)(void);
};

} // namespace sdk
