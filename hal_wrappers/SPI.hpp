#pragma once
#include "base.hpp"
#include "pinmap.hpp"

namespace sdk {

class SPI {
public:
    enum Mode {
        MODE_0 = 0, // CPOL=0, CPHA=0
        MODE_1 = 1, // CPOL=0, CPHA=1
        MODE_2 = 2, // CPOL=1, CPHA=0
        MODE_3 = 3  // CPOL=1, CPHA=1
    };

    enum BitOrder {
        MSB_FIRST = 0,
        LSB_FIRST = 1
    };

    SPI(Pin mosi, Pin miso, Pin sclk, uint32_t frequency = 1000000, 
        Mode mode = MODE_0, BitOrder bit_order = MSB_FIRST) {
        _mosi = mosi;
        _miso = miso;
        _sclk = sclk;
        
        const SPIPinMap* pinmap = find_spi_pinmap(sclk, miso, mosi);
        SDK_ASSERT(pinmap != nullptr);
        
        _spi_instance = pinmap->instance;
        _af = pinmap->af;
        
        enable_gpio_clock(_mosi.port);
        enable_gpio_clock(_miso.port);
        enable_gpio_clock(_sclk.port);
        enable_spi_clock(_spi_instance);
        
        configure_pins();
        configure_spi(frequency, mode, bit_order);
    }

    ~SPI() {
        HAL_SPI_DeInit(&_hspi);
    }

    uint8_t write(uint8_t value) {
        uint8_t rx_data = 0;
        HAL_SPI_TransmitReceive(&_hspi, &value, &rx_data, 1, 1000);
        return rx_data;
    }

    bool write(const uint8_t* data, uint16_t length, uint32_t timeout = 1000) {
        return HAL_SPI_Transmit(&_hspi, const_cast<uint8_t*>(data), length, timeout) == HAL_OK;
    }

    bool read(uint8_t* data, uint16_t length, uint32_t timeout = 1000) {
        return HAL_SPI_Receive(&_hspi, data, length, timeout) == HAL_OK;
    }

    bool transfer(const uint8_t* tx_data, uint8_t* rx_data, uint16_t length, uint32_t timeout = 1000) {
        return HAL_SPI_TransmitReceive(&_hspi, const_cast<uint8_t*>(tx_data), rx_data, length, timeout) == HAL_OK;
    }

    void frequency(uint32_t hz) {
        _hspi.Init.BaudRatePrescaler = calculate_prescaler(hz);
        HAL_SPI_Init(&_hspi);
    }

    void format(Mode mode, BitOrder bit_order = MSB_FIRST) {
        configure_mode(mode);
        _hspi.Init.FirstBit = (bit_order == MSB_FIRST) ? SPI_FIRSTBIT_MSB : SPI_FIRSTBIT_LSB;
        HAL_SPI_Init(&_hspi);
    }

    SPI_HandleTypeDef* get_handle() { return &_hspi; }

private:
    Pin _mosi;
    Pin _miso;
    Pin _sclk;
    SPI_HandleTypeDef _hspi{};
    SPI_TypeDef* _spi_instance = nullptr;
    uint8_t _af = 0;

    void configure_pins() {
        GPIO_InitTypeDef init{};
        init.Mode = GPIO_MODE_AF_PP;
        init.Pull = GPIO_NOPULL;
        init.Speed = GPIO_SPEED_FREQ_VERY_HIGH;
        init.Alternate = _af;

        init.Pin = (1 << _sclk.pin);
        HAL_GPIO_Init(_sclk.port, &init);

        init.Pin = (1 << _mosi.pin);
        HAL_GPIO_Init(_mosi.port, &init);

        init.Pin = (1 << _miso.pin);
        HAL_GPIO_Init(_miso.port, &init);
    }

    void configure_spi(uint32_t frequency, Mode mode, BitOrder bit_order) {
        _hspi.Instance = _spi_instance;
        _hspi.Init.Mode = SPI_MODE_MASTER;
        _hspi.Init.Direction = SPI_DIRECTION_2LINES;
        _hspi.Init.DataSize = SPI_DATASIZE_8BIT;
        _hspi.Init.NSS = SPI_NSS_SOFT;
        _hspi.Init.BaudRatePrescaler = calculate_prescaler(frequency);
        _hspi.Init.FirstBit = (bit_order == MSB_FIRST) ? SPI_FIRSTBIT_MSB : SPI_FIRSTBIT_LSB;
        _hspi.Init.TIMode = SPI_TIMODE_DISABLE;
        _hspi.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
        
        configure_mode(mode);
        
        HAL_StatusTypeDef status = HAL_SPI_Init(&_hspi);
        SDK_ASSERT(status == HAL_OK);
    }

    void configure_mode(Mode mode) {
        switch (mode) {
            case MODE_0:
                _hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
                _hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
                break;
            case MODE_1:
                _hspi.Init.CLKPolarity = SPI_POLARITY_LOW;
                _hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
                break;
            case MODE_2:
                _hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
                _hspi.Init.CLKPhase = SPI_PHASE_1EDGE;
                break;
            case MODE_3:
                _hspi.Init.CLKPolarity = SPI_POLARITY_HIGH;
                _hspi.Init.CLKPhase = SPI_PHASE_2EDGE;
                break;
        }
    }

    uint32_t calculate_prescaler(uint32_t frequency) {
        uint32_t apb_clock = get_apb_clock(_spi_instance);
        
        if (frequency >= apb_clock / 2)   return SPI_BAUDRATEPRESCALER_2;
        if (frequency >= apb_clock / 4)   return SPI_BAUDRATEPRESCALER_4;
        if (frequency >= apb_clock / 8)   return SPI_BAUDRATEPRESCALER_8;
        if (frequency >= apb_clock / 16)  return SPI_BAUDRATEPRESCALER_16;
        if (frequency >= apb_clock / 32)  return SPI_BAUDRATEPRESCALER_32;
        if (frequency >= apb_clock / 64)  return SPI_BAUDRATEPRESCALER_64;
        if (frequency >= apb_clock / 128) return SPI_BAUDRATEPRESCALER_128;
        return SPI_BAUDRATEPRESCALER_256;
    }
};

} // namespace sdk
