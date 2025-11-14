
#ifndef AIRBRAKES_SDK_SCOPED_LOCK_H_
#define AIRBRAKES_SDK_SCOPED_LOCK_H_

#include <sdk/mutex.h>

namespace sdk {

/**
 * Represents a lock on a mutex that lasts the lifetime of this object.
 */
class scoped_lock {
public:
    explicit scoped_lock(mutex &mutex): locked_mutex(mutex)
    {
        locked_mutex.lock();
    }

    ~scoped_lock()
    {
        locked_mutex.unlock();
    }

    // move-only, constructor-only semantics
    scoped_lock(const scoped_lock &) = delete;
    scoped_lock(scoped_lock &&) = default;

private:
    mutex &locked_mutex;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_SCOPED_LOCK_H_
