
#ifndef AIRBRAKES_SDK_SIGNAL_H_
#define AIRBRAKES_SDK_SIGNAL_H_

#include <sdk/result.h>

#include <FreeRTOS.h>
#include <task.h>

namespace sdk {

/**
 * A class representing a blocked "signal", which can be used to synchronize a
 * thread from an ISR. Only one thread can be blocked with this signal at a
 * time.
 */
class signal {
public:

    enum class error {
        OK,
        FULL,
    };

public:

    signal() : blocked_task(nullptr)
    {
    }
    
    /** Blocks the calling thread until this signal is unblocked. */
    success<error> block();
    /**
     * Unblocks this signal (only callable from an interrupt). Has no error
     * handling to ensure minimum execution time from interrupt.
     */
    void unblock_from_isr();

    /** Returns true if this signal is currently blocking a thread. */
    bool is_full()
    {
        return blocked_task != nullptr;
    }

private:
    TaskHandle_t blocked_task;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_SIGNAL_H_
