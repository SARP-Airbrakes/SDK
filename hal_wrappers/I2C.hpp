#pragma once
#include "base.hpp"
#include "pinmap.hpp"

namespace sdk {

class I2C {
public:
    I2C(Pin sda, Pin scl, uint32_t frequency = 100000) {
        _sda = sda;
        _scl = scl;
        
        const I2CPinMap* pinmap = find_i2c_pinmap(scl, sda);
        SDK_ASSERT(pinmap != nullptr);
        
        _i2c_instance = pinmap->instance;
        _af = pinmap->af;
        
        enable_gpio_clock(_sda.port);
        enable_gpio_clock(_scl.port);
        enable_i2c_clock(_i2c_instance);
        
        configure_pins();
        configure_i2c(frequency);
    }

    ~I2C() {
        HAL_I2C_DeInit(&_hi2c);
    }

    bool write(uint8_t address, const uint8_t* data, uint16_t length, uint32_t timeout = 1000) {
        return HAL_I2C_Master_Transmit(&_hi2c, address << 1, const_cast<uint8_t*>(data), length, timeout) == HAL_OK;
    }

    bool read(uint8_t address, uint8_t* data, uint16_t length, uint32_t timeout = 1000) {
        return HAL_I2C_Master_Receive(&_hi2c, address << 1, data, length, timeout) == HAL_OK;
    }

    bool write_read(uint8_t address, const uint8_t* tx_data, uint16_t tx_length, 
                    uint8_t* rx_data, uint16_t rx_length, uint32_t timeout = 1000) {
        if (!write(address, tx_data, tx_length, timeout)) return false;
        return read(address, rx_data, rx_length, timeout);
    }

    bool write_register(uint8_t address, uint8_t reg, uint8_t value, uint32_t timeout = 1000) {
        return HAL_I2C_Mem_Write(&_hi2c, address << 1, reg, I2C_MEMADD_SIZE_8BIT, &value, 1, timeout) == HAL_OK;
    }

    bool read_register(uint8_t address, uint8_t reg, uint8_t* value, uint32_t timeout = 1000) {
        return HAL_I2C_Mem_Read(&_hi2c, address << 1, reg, I2C_MEMADD_SIZE_8BIT, value, 1, timeout) == HAL_OK;
    }

    bool read_registers(uint8_t address, uint8_t reg, uint8_t* data, uint16_t length, uint32_t timeout = 1000) {
        return HAL_I2C_Mem_Read(&_hi2c, address << 1, reg, I2C_MEMADD_SIZE_8BIT, data, length, timeout) == HAL_OK;
    }

    bool is_device_ready(uint8_t address, uint32_t trials = 3, uint32_t timeout = 1000) {
        return HAL_I2C_IsDeviceReady(&_hi2c, address << 1, trials, timeout) == HAL_OK;
    }

    I2C_HandleTypeDef* get_handle() { return &_hi2c; }

private:
    Pin _sda;
    Pin _scl;
    I2C_HandleTypeDef _hi2c{};
    I2C_TypeDef* _i2c_instance = nullptr;
    uint8_t _af = 0;

    void configure_pins() {
        GPIO_InitTypeDef init{};
        init.Mode = GPIO_MODE_AF_OD;
        init.Pull = GPIO_PULLUP;
        init.Speed = GPIO_SPEED_FREQ_HIGH;
        init.Alternate = _af;

        init.Pin = (1 << _scl.pin);
        HAL_GPIO_Init(_scl.port, &init);

        init.Pin = (1 << _sda.pin);
        HAL_GPIO_Init(_sda.port, &init);
    }

    void configure_i2c(uint32_t frequency) {
        _hi2c.Instance = _i2c_instance;
        _hi2c.Init.ClockSpeed = frequency;
        _hi2c.Init.DutyCycle = I2C_DUTYCYCLE_2;
        _hi2c.Init.OwnAddress1 = 0;
        _hi2c.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
        _hi2c.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
        _hi2c.Init.OwnAddress2 = 0;
        _hi2c.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
        _hi2c.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
        
        HAL_StatusTypeDef status = HAL_I2C_Init(&_hi2c);
        SDK_ASSERT(status == HAL_OK);
    }
};

} // namespace sdk
