
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

    /**
     * Prepares this signal to block. If unblock_from_isr is called before
     * #block() is then called, then the next call to #block() does not actually
     * block. This is for I2C and SPI non-blocking modes.
     */
    void prepare_block();

    /** Returns true if this signal is currently blocking a thread. */
    bool is_full()
    {
        return blocked_task != nullptr;
    }

private:

    enum class state {
        NONE,
        PREPARED,
        FINISHED_EARLY,
    };

    TaskHandle_t blocked_task;
    state block_state;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_SIGNAL_H_
