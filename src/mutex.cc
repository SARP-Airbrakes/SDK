
#include <sdk/mutex.h>

namespace sdk {

mutex::mutex() {
    handle = xSemaphoreCreateMutexStatic(&buffer);
}

void mutex::lock()
{
    xSemaphoreTake(handle, portMAX_DELAY);
}

mutex::status mutex::try_lock(uint32_t timeout_ms)
{
    return xSemaphoreTake(handle, timeout_ms / portTICK_PERIOD_MS) == pdTRUE ? status::OK : status::IN_USE;
}

mutex::status mutex::unlock()
{
    return xSemaphoreGive(handle) == pdTRUE ? status::OK : status::ERROR;
}

}
