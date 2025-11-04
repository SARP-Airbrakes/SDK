
#ifndef AIRBRAKES_SDK_SCOPED_LOCK_H_
#define AIRBRAKES_SDK_SCOPED_LOCK_H_

#include <sdk/mutex.h>

namespace sdk {

class scoped_lock {
public:
    explicit scoped_lock(mutex &mutex);
    ~scoped_lock();

    // move-only semantics
    scoped_lock(const scoped_lock &) = delete;
    scoped_lock(scoped_lock &&) = default;
    scoped_lock &operator=(const scoped_lock &) = delete;
    scoped_lock &operator=(scoped_lock &&) = default;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_SCOPED_LOCK_H_
