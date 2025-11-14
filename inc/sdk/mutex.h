
#ifndef AIRBRAKES_SDK_MUTEX_H_
#define AIRBRAKES_SDK_MUTEX_H_

#include <FreeRTOS.h>
#include <semphr.h>

namespace sdk {

/**
 * Represents mutual exclusion over ownership of a resource.
 */
class mutex {
public:
    
    enum class status {
        OK,
        IN_USE,
        ERROR,
    };

public:
    mutex();
    
    // move-only semantics
    mutex(const mutex &) = delete;
    mutex(mutex &&) = default;
    mutex &operator=(const mutex &) = delete;
    mutex &operator=(mutex &&) = default;

    /**
     * If this mutex is unlocked, attempts to lock this mutex. Else, instead
     * blocks the current thread indefinitely until the lock on the mutex is
     * relieved.
     */
    void lock();

    /**
     * If this mutex is unlocked, attempts to lock this mutex. Else, instead
     * blocks the current thread for up to `timeout_ms` milliseconds until the
     * lock on the mutex is relieved.
     *
     * Returns status::OK if successfully locked, status::IN_USE if the mutex is
     * unavailable.
     */
     status try_lock(uint32_t timeout_ms);

    /**
     * If this mutex is locked by the current thread, relinquish the lock. If
     * not, undefined behavior may occur.
     *
     * Returns status::OK if the mutex was unlocked, status::ERROR if not.
     */
    status unlock();

private:

    SemaphoreHandle_t handle;
    StaticSemaphore_t buffer; // mutex buffer
};

} // namespace sdk;

#endif // AIRBRAKES_SDK_MUTEX_H_
