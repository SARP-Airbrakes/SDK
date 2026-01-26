
#ifndef AIRBRAKES_SDK_I2C_H_
#define AIRBRAKES_SDK_I2C_H_

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

#include <FreeRTOS.h>
#include <task.h>
#include <sdk/scoped_lock.h>
#include <sdk/result.h>

namespace sdk {

/**
 * A class representing a thread-safe I2C master interface that wraps around a
 * given HAL I2C interface.
 */
class i2c_master {
public:

    /**
     * Status codes from the I2C interface.
     */
    enum class error {
        OK,
        ERROR,
    };

public:

    /** get a sdk::i2c_master object associated with a handle */
    static i2c_master *from_handle(I2C_HandleTypeDef *handle);

    /**
     * Creates a new `i2c_master` class from a given I2C HAL handle.
     */
    i2c_master(I2C_HandleTypeDef *handle) : blocked_task(nullptr), handle(handle)
    {
        /*
         * this is completely evil but since there is no userdata field of the
         * handle we can store some data in some unused field (hdmatx for
         * example). stores *this for callbacks
         */
        handle->hdmatx = (DMA_HandleTypeDef *) this;
    }

    /**
     * Initiates a read from a `reg_address` using the `device_address` given.
     * Reads `data_size` bytes into `data`. If `mem_16bit` is true, then the
     * read will use a 10-bit `reg_address` to communicate with the device.
     *
     * Importantly, the `device_address` is the address shifted by one bit left,
     * not the 7-bit or 10-bit address that datasheets usually list.
     */
    success<error> read(uint16_t device_address, uint16_t reg_address, uint8_t
            *data, uint16_t data_size, bool mem_16bit);

    /**
     * Initiates a write to a `reg_address` using the `device_address` given.
     * Writes `data_size` bytes from `data`. If `mem_16bit` is true, then the
     * write will use a 16-bit `reg_address` to communicate with the device.
     *
     * Importantly, the `device_address` is the full address, not the 7-bit
     * address that datasheets usually list.
     */
    success<error> write(uint16_t device_address, uint16_t reg_address, uint8_t *data,
            uint16_t data_size, bool mem_16bit);

    void unblock_from_isr();

private:

    TaskHandle_t blocked_task;
    I2C_HandleTypeDef *handle;
    mutex interface_mutex;
};

} // namespace sdk

#endif // AIRBRAKES_SDK_I2C_H_
