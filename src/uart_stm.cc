
#include <cstdint>
#include <cstdio>
#include <sdk/uart.h>

namespace sdk {

success<uart_buffered::error> uart_buffered::set_baud(uint32_t baud)
{
    if (rx_state != state::IDLE)
        return error::BUSY;

    uint32_t pclk_freq = 0;
    // see stm32f4xx_hal_uart.c, lines 3770-3773
    if ((handle->Instance == USART1) || (handle->Instance == USART6)) {
        pclk_freq = HAL_RCC_GetPCLK2Freq();
    } else {
        pclk_freq = HAL_RCC_GetPCLK1Freq();
    }
    
    if (handle->Init.OverSampling == UART_OVERSAMPLING_8) {
        handle->Instance->BRR = UART_BRR_SAMPLING8(pclk_freq, baud);
    } else {
        handle->Instance->BRR = UART_BRR_SAMPLING16(pclk_freq, baud);
    }
    return success<error>();
}

success<uart_buffered::error> uart_buffered::read()
{
    if (rx_state != state::IDLE)
        return error::BUSY;

    if ((write_index + 1) % BUFFER_SIZE == read_index)
        return error::FULL;

    rx_state = state::OPERATING;

    // read one byte
    auto status = HAL_UART_Receive_IT(handle, &buffer[write_index], 1);
    write_index = (write_index + 1) % BUFFER_SIZE;

    return status != HAL_OK ? error::FAIL : error::OK;
}

success<uart_buffered::error> uart_buffered::transmit(const uint8_t *data,
        size_t data_size)
{
    if (interface_tx_signal.is_full() || tx_state != state::IDLE)
        return error::BUSY;

    tx_state = state::OPERATING;

    interface_tx_signal.prepare_block();
    auto status = HAL_UART_Transmit_IT(handle, data, data_size);

    // block current thread
    RESULT_UNWRAP_OR(interface_tx_signal.block(), error::BUSY);

    if (tx_state == state::ERROR) {
        tx_state = state::IDLE;
        return error::FAIL;
    }

    return status != HAL_OK ? error::FAIL : error::OK;
}

void uart_buffered::stop()
{ 
    if (rx_state == state::OPERATING) {
        rx_state = state::STOPPING;
    }
}

void uart_buffered::receive_complete_from_isr() 
{
    if (rx_state == state::OPERATING) {
        // if there is space in the buffer
        if ((write_index + 1) % BUFFER_SIZE != read_index) {
            uint8_t received_byte = buffer[write_index];
            /* 
             * TODO: No error handling here, but this is to be called in an
             * interrupt. I assume that there will not ever be errors here to worry
             * about unless there are serious connectivity issues.
             */
            HAL_UART_Receive_IT(handle, buffer + write_index, 1);

            write_index = (write_index + 1) % BUFFER_SIZE;
            await_size += 1;

            if (interface_rx_signal.is_full() && received_byte == target_byte)
                interface_rx_signal.unblock_from_isr();
        } else {
            /* we're full, pass */
        }
    } else if (rx_state == state::STOPPING) {
        rx_state = state::IDLE;

        // make sure we aren't blocking anything
        if (interface_rx_signal.is_full()) 
            interface_rx_signal.unblock_from_isr();
    }
}

void uart_buffered::transmit_complete_from_isr()
{
    tx_state = state::IDLE;

    if (interface_tx_signal.is_full()) 
        interface_tx_signal.unblock_from_isr();
}

void uart_buffered::error_from_isr()
{
    rx_state = state::ERROR;

    if (interface_rx_signal.is_full()) 
        interface_rx_signal.unblock_from_isr();
    if (interface_tx_signal.is_full()) 
        interface_tx_signal.unblock_from_isr();
}

bool uart_buffered::is_full()
{
    return ((write_index + 1) % BUFFER_SIZE == read_index);
}

result<size_t, uart_buffered::error> uart_buffered::next(uint8_t byte)
{
    if (interface_rx_signal.is_full())
        return error::BUSY;

    // else, block
    target_byte = byte;

    // block current thread
    RESULT_UNWRAP_OR(interface_rx_signal.block(), error::BUSY);
    size_t out = await_size;

    if (rx_state == state::ERROR) {
        rx_state = state::IDLE;
        return error::FAIL;
    }
    
    await_size = 0;
    return out;
}

result<size_t, uart_buffered::error> uart_buffered::move(uint8_t *out, size_t size)
{
    size_t number = 0;
    /* invalidate await size state */
    await_size = 0;
    for (size_t i = 0; read_index != write_index; read_index = (read_index + 1) % BUFFER_SIZE) {
        if (i > size)
            return number;
        out[i++] = buffer[read_index];
        number++;
    }
    return number;
}

} // namespace sdk
