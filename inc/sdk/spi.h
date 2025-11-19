
#ifndef AIRBRAKES_SDK_SPI_H_
#define AIRBRAKES_SDK_SPI_H_

#include <stm32f4xx_hal.h>
#include <sdk/mutex.h>

namespace sdk {

/**
 * A class representing a thread-safe SPI interface that wraps around a given
 * HAL SPI interface. Does not manage chip select lines -- that must be
 * externally managed.
 */
class spi {
public:

    /**
     * Status codes from the SPI interface.
     */
    enum class status {
        OK,
        ERROR,
    };

public:

    /**
     * Creates a new `spi` class from a given SPI HAL interface
     */
    spi(SPI_HandleTypeDef *handle) : handle(handle) {}

    /**
     * Receives `size` bytes into `data` from the interface.
     */
    status receive(uint8_t *data, uint16_t size);
    /**
     * Transmits `size` bytes from `data` through the interface;
     */
    status transmit(uint8_t *data, uint16_t size);

private:
    SPI_HandleTypeDef *handle;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_SPI_H_
