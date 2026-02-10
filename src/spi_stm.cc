
#include <sdk/spi.h>
#include <sdk/scoped_lock.h>

namespace sdk {

spi *spi::from_handle(SPI_HandleTypeDef *handle)
{
    return (spi *) handle->hdmatx;
}

success<spi::error> spi::receive(uint8_t *data, uint16_t size)
{
    if (interface_signal.is_full())
        return error::BUSY;

    scoped_lock lock(interface_mutex);

    HAL_StatusTypeDef status = HAL_SPI_Receive_IT(
        handle,
        data,
        size
    );
    RESULT_UNWRAP_OR(interface_signal.block(), error::BUSY);
    return status == HAL_OK ? error::OK : error::ERROR;
}

success<spi::error> spi::transmit(const uint8_t *data, uint16_t size)
{
    if (interface_signal.is_full())
        return error::BUSY;

    scoped_lock lock(interface_mutex);

    HAL_StatusTypeDef status = HAL_SPI_Transmit_IT(
        handle,
        data,
        size
    );
    RESULT_UNWRAP_OR(interface_signal.block(), error::BUSY);
    return status == HAL_OK ? error::OK : error::ERROR;
}

void spi::unblock_from_isr()
{
    interface_signal.unblock_from_isr();
}

} // namespace sdk
