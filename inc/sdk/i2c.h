
#ifndef AIRBRAKES_SDK_I2C_H_
#define AIRBRAKES_SDK_I2C_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include <sdk/scoped_lock.h>

namespace sdk {

/**
 * A class representing a thread-safe I2C master interface.
 */
class i2c_master {
public:

    /**
     * Status codes from the I2C interface.
     */
    enum class status {
        OK,
        ERROR,
    };

public:

    /**
     * Creates a new `i2c_master` class from a given I2C handle and an
     * associated mutex.
     */
    i2c_master(I2C_HandleTypeDef *handle, mutex &interface_mutex) :
        handle(handle), interface_mutex(interface_mutex) {}

    /**
     * Initiates a read from a `reg_address` using the `device_address` given.
     * Reads `data_size` bytes into `data`. If `mem_16bit` is true, then the
     * read will use a 16-bit `reg_address` to communicate with the device.
     *
     * Importantly, the `device_address` is the full address, not the 7-bit
     * address that datasheets usually list.
     */
    status read(uint16_t device_address, uint16_t reg_address, uint8_t *data,
            uint16_t data_size, bool mem_16bit);

    /**
     * Initiates a write to a `reg_address` using the `device_address` given.
     * Writes `data_size` bytes from `data`. If `mem_16bit` is true, then the
     * write will use a 16-bit `reg_address` to communicate with the device.
     *
     * Importantly, the `device_address` is the full address, not the 7-bit
     * address that datasheets usually list.
     */
    status write(uint16_t device_address, uint16_t reg_address, uint8_t *data,
            uint16_t data_size, bool mem_16bit);

private:

    I2C_HandleTypeDef *handle;
    mutex &interface_mutex;
};

} // namespace sdk

#endif // AIRBRAKES_SDK_I2C_H_
