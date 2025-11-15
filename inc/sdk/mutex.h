
#ifndef AIRBRAKES_SDK_MUTEX_H_
#define AIRBRAKES_SDK_MUTEX_H_

#include <cmsis_os2.h>

namespace sdk {

/**
 * Represents mutual exclusion over ownership of a resource. Wraps around an
 * existing mutex that was generated with STM32CubeMX, and gives it nicer
 * semantics.
 */
class mutex {
public:
    
    enum class status {
        OK,
        IN_USE,
        ERROR,
    };

public:

    mutex(osMutexId_t handle) : handle(handle) {}

    mutex(const mutex &) = delete;
    mutex(mutex &&) = default;
    mutex &operator=(const mutex &) = delete;
    mutex &operator=(mutex &&) = default;

    /**
     * If this mutex is unlocked, attempts to lock this mutex. Else, instead
     * blocks the current thread indefinitely until the lock on the mutex is
     * relieved.
     *
     * This method is dangerous, as if the mutex is in an invalid state, the
     * thread may be put to sleep forever.
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
    osMutexId_t handle;

};

} // namespace sdk;

#endif // AIRBRAKES_SDK_MUTEX_H_
