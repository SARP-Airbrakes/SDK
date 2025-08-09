#pragma once
#include <stdint.h>
#include "../base.hpp"

namespace sdk::bus {

// Blocking I2C convenience (7-bit addr; pass 0x68 not 0xD0)
class I2C {
public:
  explicit I2C(I2C_HandleTypeDef* hi2c, uint32_t timeout_ms = 100)
  : hi2c_(hi2c), tmo_(timeout_ms) {}

  inline HAL_StatusTypeDef write(uint8_t addr7, const uint8_t* data, uint16_t len) const {
    return HAL_I2C_Master_Transmit(hi2c_, static_cast<uint16_t>(addr7 << 1),
                                   const_cast<uint8_t*>(data), len, tmo_);
  }

  inline HAL_StatusTypeDef read(uint8_t addr7, uint8_t* data, uint16_t len) const {
    return HAL_I2C_Master_Receive(hi2c_, static_cast<uint16_t>(addr7 << 1), data, len, tmo_);
  }

  inline HAL_StatusTypeDef mem_write(uint8_t addr7, uint16_t reg, uint16_t reg_size,
                                     const uint8_t* data, uint16_t len) const {
    return HAL_I2C_Mem_Write(hi2c_, static_cast<uint16_t>(addr7 << 1),
                             reg, reg_size, const_cast<uint8_t*>(data), len, tmo_);
  }

  inline HAL_StatusTypeDef mem_read(uint8_t addr7, uint16_t reg, uint16_t reg_size,
                                    uint8_t* data, uint16_t len) const {
    return HAL_I2C_Mem_Read(hi2c_, static_cast<uint16_t>(addr7 << 1),
                            reg, reg_size, data, len, tmo_);
  }

  // Optional non-blocking
  inline HAL_StatusTypeDef write_dma(uint8_t addr7, const uint8_t* data, uint16_t len) const {
    return HAL_I2C_Master_Transmit_DMA(hi2c_, static_cast<uint16_t>(addr7 << 1),
                                       const_cast<uint8_t*>(data), len);
  }
  inline HAL_StatusTypeDef read_dma(uint8_t addr7, uint8_t* data, uint16_t len) const {
    return HAL_I2C_Master_Receive_DMA(hi2c_, static_cast<uint16_t>(addr7 << 1), data, len);
  }

  inline bool is_ready(uint8_t addr7, uint32_t trials = 1) const {
    return HAL_I2C_IsDeviceReady(hi2c_, static_cast<uint16_t>(addr7 << 1), trials, tmo_) == HAL_OK;
  }

private:
  I2C_HandleTypeDef* hi2c_;
  uint32_t tmo_;
};

} // namespace sdk::bus
