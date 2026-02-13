
#include <sdk/signal.h>

namespace sdk {

success<signal::error> signal::block()
{
    if (is_full())
        return error::FULL;
    blocked_task = xTaskGetCurrentTaskHandle();
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    return success<error>();
}

void signal::unblock_from_isr()
{
    if (blocked_task == nullptr)
        return; // ignore

    BaseType_t task_woken;
    vTaskNotifyGiveFromISR(blocked_task, &task_woken);
    blocked_task = nullptr;
    portYIELD_FROM_ISR(task_woken);
}

} // namespace sdk
