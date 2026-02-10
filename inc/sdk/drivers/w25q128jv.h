
#ifndef AIRBRAKES_SDK_DRIVER_W25Q128JV_H_
#define AIRBRAKES_SDK_DRIVER_W25Q128JV_H_

#include <sdk/queue.h>
#include <sdk/unique_pin.h>
#include <sdk/spi.h>

#include <utility>

namespace sdk {

/**
 * A class representing a driver for the W25Q128JV NOR flash chip.
 */
class w25q128jv {
public: // constants

    static constexpr int PAGE_PROGRAM_COMMAND = 0x02;
    static constexpr int WRITE_ENABLE_COMMAND = 0x06;

public:

    /**
     * Constructs a new `w25q128jv` class using the provided SPI interface.
     */
    explicit w25q128jv(spi &interface, unique_pin pin) : interface(interface),
            pin(std::move(pin)), write_queue(8)
    {
    }

    /**
     * Updates the internal driver state. To only be called from a separate
     * driver thread.
     */
    void update();

    void read(uint32_t address, uint8_t *buffer, uint32_t size);
    /**
     * Writes `size` bytes of `buffer` to the flash memory starting at
     * `address`. Thread-safe blocking.
     */
    void write(uint32_t address, const uint8_t *buffer, uint32_t size);

private:
    
    struct write_command { // 12 bytes
        uint32_t address;
        uint8_t *data;
        uint32_t data_size;
    };

    struct read_command {

    };

private:

    // enables write
    void enable_write();

    void enable_chip(); // drive cs low
    void disable_chip(); // drive cs high

    // writes to the chip directly
    void write(uint32_t address, uint8_t *data, uint32_t data_size);


    // reads the BUSY bit from the chip
    bool is_busy();

private:
    spi &interface;
    unique_pin pin;
    mutex state_mutex;
    queue<write_command> write_queue;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_DRIVER_W25Q128JV_H_
