
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

    interface_signal.prepare_block();
    HAL_StatusTypeDef status = HAL_SPI_Receive_IT(
        handle,
        data,
        size
    );

    if (status != HAL_OK)
        return error::ERROR;

    RESULT_UNWRAP_OR(interface_signal.block(), error::BUSY);

    success<error> out = error_flag ? error::ERROR : error::OK;
    error_flag = false;
    return out;
}

success<spi::error> spi::transmit(const uint8_t *data, uint16_t size)
{
    if (interface_signal.is_full())
        return error::BUSY;

    scoped_lock lock(interface_mutex);

    interface_signal.prepare_block();
    HAL_StatusTypeDef status = HAL_SPI_Transmit_IT(
        handle,
        data,
        size
    );

    if (status != HAL_OK)
        return error::ERROR;

    RESULT_UNWRAP_OR(interface_signal.block(), error::BUSY);

    success<error> out = error_flag ? error::ERROR : error::OK;
    error_flag = false;
    return out;
}

success<spi::error> spi::transmit_receive(const uint8_t *tx_data, uint8_t
        *rx_data, uint16_t size)
{
    if (interface_signal.is_full())
        return error::BUSY;

    scoped_lock lock(interface_mutex);

    interface_signal.prepare_block();
    HAL_StatusTypeDef status = HAL_SPI_TransmitReceive_IT(
        handle,
        tx_data,
        rx_data,
        size
    );

    if (status != HAL_OK)
        return error::ERROR;

    RESULT_UNWRAP_OR(interface_signal.block(), error::BUSY);

    success<error> out = error_flag ? error::ERROR : error::OK;
    error_flag = false;
    return out;
}

void spi::unblock_from_isr()
{
    interface_signal.unblock_from_isr();
}

void spi::error_from_isr()
{
    error_flag = true;
    interface_signal.unblock_from_isr();
}

} // namespace sdk
