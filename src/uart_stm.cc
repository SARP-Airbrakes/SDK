
#include <sdk/uart.h>

namespace sdk {

success<uart_buffered::error> uart_buffered::read()
{
    if (index >= BUFFER_SIZE)
        return success<error>(error::FULL);
    auto status = HAL_UART_Receive_IT(handle, &buffer[index++], sizeof(*buffer));
    return status != HAL_OK ? error::FAIL : error::OK;
}

void uart_buffered::stop()
{
    if (reader_state == state::READING) {
        reader_state = state::STOPPING;
    }
}

void uart_buffered::receive() 
{
    if (reader_state == state::READING) {
        // if there is space in the buffer
        if ((write_index + 1) % BUFFER_SIZE != read_index) {
            uint8_t received_byte = buffer[write_index];
            /* 
             * TODO: No error handling here, but this is to be called in an
             * interrupt. I assume that there will not ever be errors here to worry
             * about unless there are serious connectivity issues.
             */
            HAL_UART_Receive_IT(handle, &buffer[write_index], sizeof(*buffer));

            write_index = (write_index + 1) % BUFFER_SIZE;
            await_size += 1;

            if (blocked_task != nullptr && received_byte == target_byte) {
                BaseType_t task_woken;
                vTaskNotifyGiveFromISR(blocked_task, &task_woken);
                blocked_task = nullptr;
                portYIELD_FROM_ISR(task_woken);
            }
        } else {
            reader_state = state::FULL;
        }
    } else if (reader_state == state::STOPPING) {
        reader_state = state::IDLE;

        // unblock a blocked task when stopping
        if (blocked_task != nullptr) {
            BaseType_t task_woken;
            vTaskNotifyGiveFromISR(blocked_task, &task_woken);
            blocked_task = nullptr;
            portYIELD_FROM_ISR(task_woken);
        }
    }
}

result<size_t, uart_buffered::error> uart_buffered::await(uint8_t byte)
{
    if (blocked_task != nullptr)
        return error::WAITING;
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
