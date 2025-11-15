
#ifndef AIRBRAKES_SDK_SPI_H_
#define AIRBRAKES_SDK_SPI_H_

#include <stm32f4xx_hal.h>
#include <cmsis_os2.h>

#include <sdk/mutex.h>

namespace sdk {

/**
 * A class representing a thread-safe SPI interface.
 */
class spi {
public:
    enum class status {
        OK,
        ERROR,
    };

public:

    spi(SPI_HandleTypeDef *handle, mutex &interface_mutex) :
        handle(handle), interface_mutex(interface_mutex) {}

    status receive(uint8_t *data, uint16_t size);
    status transmit(uint8_t *data, uint16_t size);

private:
    SPI_HandleTypeDef *handle;
    mutex &interface_mutex;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_SPI_H_
