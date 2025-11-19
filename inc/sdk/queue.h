
#ifndef AIRBRAKES_SDK_QUEUE_H_
#define AIRBRAKES_SDK_QUEUE_H_

#include <FreeRTOS.h>
#include <queue.h>

namespace sdk {

/**
 * A class representing a thread-safe queue object.
 */
template<typename T>
class queue {
public:

    enum class status {
        // success conditions
        OK = 0,

        // failure conditions
        FULL = 1,
        EMPTY = 1
    };
public:

    /**
     * Instantiates a `queue` object with space for `length` objects of
     * `sizeof(T)`.
     */
    explicit queue(UBaseType_t length)
    {
        handle = xQueueCreate(length, sizeof(T));
    }

    /**
     * Attempts to push the given value to the back of the queue, or blocks the
     * thread up to `timeout_ms` if the queue is full. Returns `status::OK` if
     * the value was sent, else `status::FULL`.
     */
    status try_push_back(T &val, uint32_t timeout_ms)
    {
        return xQueueSendToBack(handle, &val, pdMS_TO_TICKS(timeout_ms)) ==
            pdPASS ? status::OK : status::FULL;
    }

    /**
     * Attempts to push the given value to the front of the queue, or blocks the
     * thread up to `timeout_ms` if the queue is full. Returns `status::OK` if
     * the value was sent, else `status::FULL`.
     */
    status try_push_front(T &val, uint32_t timeout_ms)
    {
        return xQueueSendToFront(handle, &val, pdMS_TO_TICKS(timeout_ms)) ==
            pdPASS ? status::OK : status::FULL;
    }

    /**
     * Pushes the given value to the back of the queue, or blocks the current
     * thread indefinitely until space is available.
     */
    void push_back(T &val)
    {
        xQueueSendToBack(handle, &val, portMAX_DELAY);
    }

    /**
     * Pushes the given value to the front of the queue, or blocks the current
     * thread indefinitely until space is available.
     */
    void push_front(T &val)
    {
        xQueueSendToFront(handle, &val, portMAX_DELAY);
    }

    /**
     * Attempts to pop (receive) the value at the front of the queue, or blocks
     * the current thread up to `timeout_ms` if the queue is empty. Returns
     * `status::OK` if a value was received, else returns `status::EMPTY`.
     */
    status try_pop(T *val, uint32_t timeout_ms)
    {
        return xQueueReceive(handle, (void *) val, pdMS_TO_TICKS(timeout_ms));
    }

    /**
     * Pops (receives) the value at the front of the queue, or blocks
     * indefinitely until there is a value to pop.
     */
    T pop()
    {
        T out;
        xQueueReceive(handle, (void *) &out, portMAX_DELAY);
        return out;
    }
    
private:
    QueueHandle_t handle;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_QUEUE_H_
