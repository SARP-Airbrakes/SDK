
#ifndef AIRBRAKES_SDK_DRIVER_W25Q16JV_H_
#define AIRBRAKES_SDK_DRIVER_W25Q16JV_H_

#include <sdk/spi.h>

namespace sdk {

/**
 * A class representing a driver for the W25Q16JV NOR flash chip.
 */
class w25q16jv {
public:

    struct state {

    };

public:

    /**
     * Constructs a new `w25q16jv` class using the provided SPI interface.
     */
    w25q16jv(spi &interface, mutex &state_mutex) : interface(interface),
            state_mutex(state_mutex) {}

    /**
     * Updates the internal driver state.
     */
    void update();



private:
    spi &interface;
    mutex &state_mutex;

};

} // namespace sdk

#endif // AIRBRAKES_SDK_DRIVER_W25Q16JV_H_
