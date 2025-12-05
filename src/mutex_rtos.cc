
#include <sdk/mutex.h>

#include <FreeRTOS.h>
#include <semphr.h>

namespace sdk {

mutex::mutex()
{
    handle = xSemaphoreCreateMutex();
}

void mutex::lock()
{
    try_lock(portMAX_DELAY);
}

mutex::status mutex::try_lock(uint32_t timeout_ms)
{
    return xSemaphoreTake((SemaphoreHandle_t) handle, pdMS_TO_TICKS(timeout_ms))
        == pdTRUE ? status::OK : status::IN_USE;
}

mutex::status mutex::unlock()
{
    return xSemaphoreGive((SemaphoreHandle_t) handle) == pdTRUE ? status::OK :
        status::ERROR;
}

void *mutex::unwrap()
{
    return handle;
}

}
