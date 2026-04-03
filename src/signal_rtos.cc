
#include <sdk/signal.h>

namespace sdk {

success<signal::error> signal::block()
{
    if (block_state == state::FINISHED_EARLY) {
        /* we didn't need to block! */
        block_state = state::NONE;
        return success<error>();
    }
    if (is_full())
        return error::FULL;
    blocked_task = xTaskGetCurrentTaskHandle();
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    block_state = state::NONE;
    return success<error>();
}

void signal::unblock_from_isr()
{
    if (blocked_task == nullptr) {
        if (block_state == state::PREPARED)
            block_state = state::FINISHED_EARLY;
        return; // ignore
    }

    BaseType_t task_woken;
    vTaskNotifyGiveFromISR(blocked_task, &task_woken);
    blocked_task = nullptr;
    portYIELD_FROM_ISR(task_woken);
}

void signal::prepare_block()
{
    block_state = state::PREPARED;
}

} // namespace sdk
