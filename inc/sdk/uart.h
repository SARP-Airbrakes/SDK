
#ifndef AIRBRAKES_SDK_UART_H_
#define AIRBRAKES_SDK_UART_H_

#include <stm32f4xx_hal.h>
#include <sdk/result.h>

#include <FreeRTOS.h>
#include <task.h>

namespace sdk {

/**
 * A class representing a thread-safe (thread-safe as in FreeRTOS) UART
 * interface that wraps around a given HAL UART interface, utilizing the
 * non-blocking interrupt mode. Stores an internal circular buffer of what is
 * read.
 */
class uart_buffered { 
public: 
    static constexpr int BUFFER_SIZE = 256;

    enum class error {
        OK,
        FAIL,
        FULL,
        WAITING,
    };

    enum class state {
        IDLE,
        READING,
        FULL,
        STOPPING
    };

public:

    /** Gets a sdk::uart_buffered object associated with a UART HAL handle. */
    static uart_buffered *from_handle(UART_HandleTypeDef *handle);

    uart_buffered(UART_HandleTypeDef *handle) : handle(handle)
    {
        /*
         * Storing "this" inside of a random (importantly unused) field in the
         * handle. Using a C-style cast here to avoid C++ reintepretation
         * mechanics.
         */
        handle->hdmatx = (DMA_HandleTypeDef *) this;
    }

    // move-only semantics (circular buffer should not be copied) */
    uart_buffered(uart_buffered &&) = default;
    uart_buffered(const uart_buffered &) = delete;
    uart_buffered &operator=(uart_buffered &&) = default;
    uart_buffered &operator=(const uart_buffered &) = delete;

    /**
     * Starts listening to the UART. Initiates a non-blocking read that will be
     * read to the buffer.
     */
    success<error> read();

    /**
     * Stops the UART after reading the next byte.
     */
    void stop();

    /**
     * To be called from an interrupt after receiving one byte.
     */
    void receive();

    /**
     * Wait for a specific byte to be transmitted (e.g. '\n', '\r'). Thread-safe
     * blocking. Only one FreeRTOS task can wait at a time. Returns the size of
     * the data frame that includes the byte.
     */
    result<size_t, error> await(uint8_t byte);

    /**
     * Moves up to `size` bytes from the buffer to the given `buf`. Clears space
     * in the circular buffer.
     */
    success<error> move(uint8_t *buf, size_t size);

private:
    UART_HandleTypeDef *handle;
    TaskHandle_t blocked_task;
    uint8_t target_byte;

    state reader_state;
    
    size_t await_size = 0;
    size_t read_index = 0;
    size_t write_index = 0;
    uint8_t buffer[BUFFER_SIZE];
};

} // namespace sdk

#endif // AIRBRAKES_SDK_UART_H_
