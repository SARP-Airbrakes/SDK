
#ifndef AIRBRAKES_SDK_MUTEX_H_
#define AIRBRAKES_SDK_MUTEX_H_

namespace sdk {

class mutex {
public:
    mutex();
    ~mutex();
    
    // move-only semantics
    mutex(const mutex &) = delete;
    mutex(mutex &&) = default;
    mutex &operator=(const mutex &) = delete;
    mutex &operator=(mutex &&) = default;
};

} // namespace sdk;

#endif // AIRBRAKES_SDK_MUTEX_H_
