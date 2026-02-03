
#include <cstdint>
#include <sdk/uart.h>

namespace sdk {

success<uart_buffered::error> uart_buffered::set_baud(uint32_t baud)
{
    if (uart_state != state::IDLE)
        return error::BUSY;

    uint32_t pclk_freq = 0;
    // see stm32f4xx_hal_uart.c, lines 3770-3773
    if ((handle->Instance == USART1) || (handle->Instance == USART6)) {
        pclk_freq = HAL_RCC_GetPCLK2Freq();
    } else {
        pclk_freq = HAL_RCC_GetPCLK1Freq();
    }
    
    if (handle->Init.OverSampling == UART_OVERSAMPLING_8) {
        handle->Instance->BRR = UART_BRR_SAMPLING8(pclk_freq, handle->Init.BaudRate);
    } else {
        handle->Instance->BRR = UART_BRR_SAMPLING16(pclk_freq, handle->Init.BaudRate);
    }
    return success<error>();
}

success<uart_buffered::error> uart_buffered::read()
{
    if (uart_state != state::IDLE)
        return error::BUSY;

    if ((write_index + 1) % BUFFER_SIZE == read_index)
        return error::FULL;

    uart_state = state::READING;

    // read one byte
    auto status = HAL_UART_Receive_IT(handle, &buffer[write_index], 1);
    write_index = (write_index + 1) % BUFFER_SIZE;

    return status != HAL_OK ? error::FAIL : error::OK;
}

success<uart_buffered::error> uart_buffered::transmit(const uint8_t *data,
        size_t data_size)
{
    if (uart_state != state::IDLE)
        return error::BUSY;

    auto status = HAL_UART_Transmit_IT(handle, data, data_size);

    // block current thread
    blocked_task = xTaskGetCurrentTaskHandle();
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    return status != HAL_OK ? error::FAIL : error::OK;
}

void uart_buffered::stop()
{
    if (uart_state == state::READING) {
        uart_state = state::STOPPING;
    }
}

void uart_buffered::receive_complete() 
{
    if (uart_state == state::READING) {
        // if there is space in the buffer
        if ((write_index + 1) % BUFFER_SIZE != read_index) {
            uint8_t received_byte = buffer[write_index];
            /* 
             * TODO: No error handling here, but this is to be called in an
             * interrupt. I assume that there will not ever be errors here to worry
             * about unless there are serious connectivity issues.
             */
            HAL_UART_Receive_IT(handle, &buffer[write_index], 1);

            write_index = (write_index + 1) % BUFFER_SIZE;
            await_size += 1;

            if (blocked_task != nullptr && received_byte == target_byte) {
                BaseType_t task_woken;
                vTaskNotifyGiveFromISR(blocked_task, &task_woken);
                blocked_task = nullptr;
                portYIELD_FROM_ISR(task_woken);
            }
        } else {
            uart_state = state::FULL;
        }
    } else if (uart_state == state::STOPPING) {
        uart_state = state::IDLE;

        // unblock a blocked task when stopping
        if (blocked_task != nullptr) {
            BaseType_t task_woken;
            vTaskNotifyGiveFromISR(blocked_task, &task_woken);
            blocked_task = nullptr;
            portYIELD_FROM_ISR(task_woken);
        }
    }
}

void uart_buffered::transmit_complete()
{
    uart_state = state::IDLE;

    // unblocked the associated blocked task
    if (blocked_task != nullptr) {
        BaseType_t task_woken;
        vTaskNotifyGiveFromISR(blocked_task, &task_woken);
        blocked_task = nullptr;
        portYIELD_FROM_ISR(task_woken);
    }
}

bool uart_buffered::is_full()
{
    return uart_state != state::FULL;
}

result<size_t, uart_buffered::error> uart_buffered::next(uint8_t byte)
{
    if (blocked_task != nullptr)
        return error::WAITING;

    // check if we don't have to block
    for (size_t i = 0; read_index != write_index; read_index = (read_index + 1) % BUFFER_SIZE) {
        i++;
        if (buffer[read_index] == byte)
            return i;
    }

    // else, block
    target_byte = byte;
    blocked_task = xTaskGetCurrentTaskHandle();
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    
    size_t out = await_size;
    await_size = 0;
    return out;
}

success<uart_buffered::error> uart_buffered::move(uint8_t *out, size_t size)
{
    /* invalidate await size state */
    await_size = 0;
    for (size_t i = 0; read_index != write_index; read_index = (read_index + 1) % BUFFER_SIZE) {
        if (i >= size)
            return success<error>();
        out[i++] = buffer[read_index];
    }
    return success<error>();
}

} // namespace sdk
