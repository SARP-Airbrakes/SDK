
#ifndef AIRBRAKES_SDK_UART_H_
#define AIRBRAKES_SDK_UART_H_

#include <stm32f4xx_hal.h>
#include <stm32f4xx_hal_uart.h>
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
    // ridiculously big for gps driver, see drivers/cdpa1616d.cc
    static constexpr int BUFFER_SIZE = 1024;

    enum class error {
        OK,
        FAIL,
        FULL,
        WAITING,
        BUSY,
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
     * Sets the baud rate of the UART interface.
     */
    success<error> set_baud(uint32_t baud);

    /**
     * Starts listening to the UART. Initiates a non-blocking read that will be
     * read to the buffer.
     */
    success<error> read();

    /**
     * Transmits data through the UART. Thread-safe blocking until transmit
     * completion.
     */
    success<error> transmit(const uint8_t *data, size_t data_size);

    /**
     * Stops the UART after reading the next byte.
     */
    void stop();

    /**
     * To be called from an interrupt after successfully receiving.
     */
    void receive_complete();

    /**
     * To be called from an interrupt after a transmission is completed.
     */
    void transmit_complete();

    /**
     * Returns true if the internal buffer is full.
     */
    bool is_full();

    /**
     * Finds a data frame that ends with the given byte, or waits until the byte
     * is received. Thread-safe blocking. Only one FreeRTOS task can wait at a
     * time. Returns the size of the data frame that includes the byte.
     */
    result<size_t, error> next(uint8_t byte);

    /**
     * Moves up to `size` bytes from start of the internal buffer to the given
     * `buf`. Clears space in the buffer.
     */
    success<error> move(uint8_t *buf, size_t size);

private:
    UART_HandleTypeDef *handle;
    TaskHandle_t blocked_task;
    uint8_t target_byte;

    state uart_state;
    
    size_t await_size = 0;
    size_t read_index = 0;
    size_t write_index = 0;
    uint8_t buffer[BUFFER_SIZE];
};

} // namespace sdk

#endif // AIRBRAKES_SDK_UART_H_
