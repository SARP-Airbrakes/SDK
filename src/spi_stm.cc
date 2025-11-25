
#include <sdk/spi.h>
#include <sdk/scoped_lock.h>

namespace sdk {

spi::status spi::receive(uint8_t *data, uint16_t size)
{
    // TODO: scoped_lock lock(interface_mutex);
    return HAL_SPI_Receive(handle, data, size,
            HAL_MAX_DELAY) == HAL_OK ? status::OK : status::ERROR;
}

spi::status spi::transmit(uint8_t *data, uint16_t size)
{
    // TODO: scoped_lock lock(interface_mutex);
    return HAL_SPI_Transmit(handle, data, size,
            HAL_MAX_DELAY) == HAL_OK ? status::OK : status::ERROR;
}

} // namespace sdk
