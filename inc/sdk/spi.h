
#ifndef AIRBRAKES_SDK_SPI_H_
#define AIRBRAKES_SDK_SPI_H_

#include <sdk/mutex.h>
#include <sdk/signal.h>
#include <sdk/result.h>

#include <stm32f4xx_hal.h>

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
    enum class error {
        OK,
        BUSY,
        ERROR,
    };

public:

    static spi *from_handle(SPI_HandleTypeDef *handle);

    /**
     * Creates a new `spi` class from a given SPI HAL interface
     */
    spi(SPI_HandleTypeDef *handle) : handle(handle)
    {
        // stores a pointer to this in an unused field of the handle.
        handle->hdmatx = (DMA_HandleTypeDef *) this;
    }

    /**
     * Receives `size` bytes into `data` from the interface.
     */
    success<error> receive(uint8_t *data, uint16_t size);
    /**
     * Transmits `size` bytes from `data` through the interface;
     */
    success<error> transmit(const uint8_t *data, uint16_t size);

    void unblock_from_isr();

private:

    SPI_HandleTypeDef *handle;
    signal interface_signal;
    mutex interface_mutex;
};

} // namespace sdk

#endif // AIRBRAKES_SDK_SPI_H_
