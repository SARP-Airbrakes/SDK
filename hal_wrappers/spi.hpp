#pragma once
#include <stdint.h>
#include "../base.hpp"

namespace sdk::bus {

// Blocking SPI; manage chip-select with GPIO outside this class.
class SPI {
public:
  explicit SPI(SPI_HandleTypeDef* hspi, uint32_t timeout_ms = 100)
  : hspi_(hspi), tmo_(timeout_ms) {}

  inline HAL_StatusTypeDef tx(const uint8_t* data, uint16_t len) const {
    return HAL_SPI_Transmit(hspi_, const_cast<uint8_t*>(data), len, tmo_);
  }

  inline HAL_StatusTypeDef rx(uint8_t* data, uint16_t len) const {
    return HAL_SPI_Receive(hspi_, data, len, tmo_);
  }

  inline HAL_StatusTypeDef txrx(const uint8_t* tx_data, uint8_t* rx_data, uint16_t len) const {
    return HAL_SPI_TransmitReceive(hspi_, const_cast<uint8_t*>(tx_data), rx_data, len, tmo_);
  }

  // Optional DMA
  inline HAL_StatusTypeDef tx_dma(const uint8_t* data, uint16_t len) const {
    return HAL_SPI_Transmit_DMA(hspi_, const_cast<uint8_t*>(data), len);
  }
  inline HAL_StatusTypeDef rx_dma(uint8_t* data, uint16_t len) const {
    return HAL_SPI_Receive_DMA(hspi_, data, len);
  }
  inline HAL_StatusTypeDef txrx_dma(const uint8_t* tx_data, uint8_t* rx_data, uint16_t len) const {
    return HAL_SPI_TransmitReceive_DMA(hspi_, const_cast<uint8_t*>(tx_data), rx_data, len);
  }

  inline bool busy() const {
    auto s = hspi_->State;
    return s == HAL_SPI_STATE_BUSY || s == HAL_SPI_STATE_BUSY_TX_RX || s == HAL_SPI_STATE_BUSY_TX || s == HAL_SPI_STATE_BUSY_RX;
  }

private:
  SPI_HandleTypeDef* hspi_;
  uint32_t tmo_;
};

} // namespace sdk::bus
