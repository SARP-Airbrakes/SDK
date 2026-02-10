
#ifndef AIRBRAKES_SDK_DRIVER_W25Q128JV_H_
#define AIRBRAKES_SDK_DRIVER_W25Q128JV_H_

#include <sdk/queue.h>
#include <sdk/unique_pin.h>
#include <sdk/scoped_pin.h>
#include <sdk/spi.h>

#include <utility>

namespace sdk {

/**
 * A class representing a driver for the W25Q128JV NOR flash chip.
 */
class w25q128jv {
public: // constants

    static constexpr int DEFAULT_POLL_ATTEMPTS = 16;

    static constexpr int PAGE_PROGRAM_COMMAND = 0x02;
    static constexpr int READ_DATA_COMMAND = 0x03;
    static constexpr int READ_STATUS_REGISTER_1_COMMAND = 0x05;
    static constexpr int WRITE_ENABLE_COMMAND = 0x06;

    enum class error {
        OK,
        SPI,
        TOO_BIG,
        BUSY,
    };

public:

    /**
     * Constructs a new `w25q128jv` class using the provided SPI interface.
     */
    explicit w25q128jv(spi &interface, unique_pin cs_pin) : interface(interface),
            cs_pin(std::move(cs_pin))
    {
    }

    /**
     * Updates the internal driver state. To only be called from a separate
     * driver thread.
     */
    void update();

    /**
     * Reads `size` bytes into `buffer` from the flash memory starting at
     * `address`. Thread-safe blocking.
     */
    success<error> read(uint32_t address, uint8_t *buffer, uint32_t size);
    /**
     * Writes `size` bytes of `buffer` to the flash memory starting at
     * `address`. Thread-safe blocking.
     */
    success<error> write(uint32_t address, const uint8_t *buffer, 
            uint32_t size);

private:

    // enables write
    success<error> enable_write();

    scoped_pin enable_chip();

    // blocks until busy bit is 0
    success<error> block_for_busy_bit(uint8_t attempts = DEFAULT_POLL_ATTEMPTS);
    result<uint8_t, error> read_status_register_1();

    // reads the BUSY bit from the chip
    bool is_busy();

private:
    spi &interface;
    unique_pin cs_pin;
    mutex state_mutex;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_DRIVER_W25Q128JV_H_
