
#ifndef AIRBRAKES_SDK_CDPA1616D_H_
#define AIRBRAKES_SDK_CDPA1616D_H_

#include <sdk/uart.h>
#include <utility>

namespace sdk {

/**
 * This class represents the driver for the CD-PA1616D GNSS patch antenna
 * module.
 */
class cdpa1616d {
public:

    using real = float;

    enum class cardinals {
        NONE,
        NORTH,
        SOUTH,
        EAST,
        WEST,
    };

    struct state {
        real latitude_degrees;
        cardinals north_south;
        real longitude_degrees;
        cardinals east_west;
        real altitude_meters;
    };

    enum class error {
        OK,
        UART,
    };

public:

    cdpa1616d(sdk::uart_buffered &&uart) : uart(std::move(uart))
    {
    }

    /**
     * Update internal driver state. Thread-safe blocking.
     */
    success<error> update();

private:

    sdk::uart_buffered uart;
};

} // namespace sdk

#endif // AIRBRAKES_SDK_CDPA1616D_H_
