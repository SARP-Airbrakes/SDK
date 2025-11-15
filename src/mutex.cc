
#include <sdk/mutex.h>

namespace sdk {

void mutex::lock()
{
   if (osMutexAcquire(handle, osWaitForever) != osOK) {
        osDelay(osWaitForever);
   }
}

mutex::status mutex::try_lock(uint32_t timeout_ms)
{
    return osMutexAcquire(handle, (timeout_ms * osKernelGetTickFreq()) / 1000) == osOK ? status::OK : status::IN_USE;
}

mutex::status mutex::unlock()
{
    return osMutexRelease(handle) == osOK ? status::OK : status::ERROR;
}

}
