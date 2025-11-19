
#ifndef AIRBRAKES_SDK_SPI_H_
#define AIRBRAKES_SDK_SPI_H_

#include <stm32f4xx_hal.h>
#include <sdk/mutex.h>

namespace sdk {

/**
 * A class representing a thread-safe SPI interface. Does not manage chip select
 * lines -- that must be externally managed.
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
     * Creates a new `spi` class from a given SPI handle and an associated
     * mutex.
     */
    spi(SPI_HandleTypeDef *handle, mutex &interface_mutex) :
        handle(handle), interface_mutex(interface_mutex) {}

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
    mutex &interface_mutex;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_SPI_H_
