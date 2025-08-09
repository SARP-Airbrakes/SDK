#pragma once
#include "../base.hpp"

namespace sdk::bus {

// ---------- DigitalOut (simple, assumes pin already configured as output) ----------
class DigitalOut {
public:
  DigitalOut(GPIO_TypeDef* port, uint16_t pin) : port_(port), pin_(pin) {}

  inline void high()        const { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET); }
  inline void low()         const { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET); }
  inline void write(bool v) const { HAL_GPIO_WritePin(port_, pin_, v ? GPIO_PIN_SET : GPIO_PIN_RESET); }
  inline void toggle()      const { HAL_GPIO_TogglePin(port_, pin_); }

  inline bool read()        const { return HAL_GPIO_ReadPin(port_, pin_) == GPIO_PIN_SET; } // read back

private:
  GPIO_TypeDef* port_;
  uint16_t pin_;
};

// ---------- DigitalIn (simple, assumes pin already configured as input) ----------
class DigitalIn {
public:
  DigitalIn(GPIO_TypeDef* port, uint16_t pin) : port_(port), pin_(pin) {}

  inline bool read() const { return HAL_GPIO_ReadPin(port_, pin_) == GPIO_PIN_SET; }

private:
  GPIO_TypeDef* port_;
  uint16_t pin_;
};

// ---------- DigitalInOut (can configure mode at runtime if you didn't via CubeMX) ----------
class DigitalInOut {
public:
  DigitalInOut(GPIO_TypeDef* port, uint16_t pin) : port_(port), pin_(pin) {}

  // Configure as push-pull output
  inline void set_output_pp(uint32_t speed = GPIO_SPEED_FREQ_LOW,
                            uint32_t pull  = GPIO_NOPULL) const {
    GPIO_InitTypeDef g{};
    g.Pin   = pin_;
    g.Mode  = GPIO_MODE_OUTPUT_PP;
    g.Pull  = pull;
    g.Speed = speed;
    HAL_GPIO_Init(port_, &g);
  }

  // Configure as input
  inline void set_input(uint32_t pull = GPIO_NOPULL) const {
    GPIO_InitTypeDef g{};
    g.Pin   = pin_;
    g.Mode  = GPIO_MODE_INPUT;
    g.Pull  = pull;
    g.Speed = GPIO_SPEED_FREQ_LOW;
    HAL_GPIO_Init(port_, &g);
  }

  // (Optional) Configure as open-drain output (e.g., I²C-style lines)
  inline void set_output_od(uint32_t speed = GPIO_SPEED_FREQ_LOW,
                            uint32_t pull  = GPIO_NOPULL) const {
    GPIO_InitTypeDef g{};
    g.Pin   = pin_;
    g.Mode  = GPIO_MODE_OUTPUT_OD;
    g.Pull  = pull;
    g.Speed = speed;
    HAL_GPIO_Init(port_, &g);
  }

  // Write/toggle/read
  inline void high()        const { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_SET); }
  inline void low()         const { HAL_GPIO_WritePin(port_, pin_, GPIO_PIN_RESET); }
  inline void write(bool v) const { HAL_GPIO_WritePin(port_, pin_, v ? GPIO_PIN_SET : GPIO_PIN_RESET); }
  inline void toggle()      const { HAL_GPIO_TogglePin(port_, pin_); }
  inline bool read()        const { return HAL_GPIO_ReadPin(port_, pin_) == GPIO_PIN_SET; }

private:
  GPIO_TypeDef* port_;
  uint16_t pin_;
};

} // namespace sdk::bus