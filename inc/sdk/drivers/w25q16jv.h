
#ifndef AIRBRAKES_SDK_DRIVER_W25Q16JV_H_
#define AIRBRAKES_SDK_DRIVER_W25Q16JV_H_

#include <sdk/queue.h>
#include <sdk/shared_pin.h>
#include <sdk/spi.h>

namespace sdk {

/**
 * A class representing a driver for the W25Q16JV NOR flash chip.
 */
class w25q16jv {
public:

    static constexpr int WRITE_QUEUE_SIZE = 8; // 12 bytes * 8 = 96 bytes

public:

    /**
     * Constructs a new `w25q16jv` class using the provided SPI interface.
     */
    explicit w25q16jv(spi &interface, shared_pin &pin) : interface(interface),
            pin(pin), write_queue(8)
    {
    }

    /**
     * Updates the internal driver state. To only be called from a separate
     * driver thread.
     */
    void update();

    void queue_read();
    /**
     * Queues a write of `data` of `data_size` bytes to the chip at `address`.
     */
    void queue_write(uint32_t address, const uint8_t *data, uint32_t data_size);

private:
    
    struct write_command { // 12 bytes
        uint32_t address;
        uint8_t *data;
        uint32_t data_size;
    };

    struct read_command {

    };

private:

    void enable_chip(); // pull cs low
    void disable_chip(); // put cs high

    // writes to the chip directly
    void write(uint32_t address, uint8_t *data, uint32_t data_size);

private:
    spi &interface;
    shared_pin &pin;
    mutex state_mutex;
    queue<write_command> write_queue;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_DRIVER_W25Q16JV_H_
