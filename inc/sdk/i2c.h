
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

    i2c_master(I2C_HandleTypeDef *handle);

    /**
     * Grab the lock associated with this interface, or block the current thread
     * until the lock is grabbed.
     */
    scoped_lock &&lock();

private:
    mutex interface_mutex;
};

} // namespace sdk

#endif // AIRBRAKES_SDK_I2C_H_
