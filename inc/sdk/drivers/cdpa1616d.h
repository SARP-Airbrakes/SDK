
#ifndef AIRBRAKES_SDK_CDPA1616D_H_
#define AIRBRAKES_SDK_CDPA1616D_H_

#include <sdk/mutex.h>
#include <sdk/queue.h>
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

    enum class gps_fix {
        NO_FIX, /* no altitude, long lat data */
        GPS_FIX, /* at least 3? */
        DIFF_GPS_FIX,
    };

    struct state {
        /* between 0-23 */
        uint8_t utc_hours;
        /* between 0-59 */
        uint8_t utc_minutes;
        real utc_seconds;
        real altitude_meters;
    };

    enum class error {
        OK,
        UART,
        INVALID_COMMAND
    };

public:

    cdpa1616d(sdk::uart_buffered &&uart) : uart(std::move(uart))
    {
    }

    /**
     * Sets sensible default configurations (blocks the thread), then starts
     * reading from the UART (non-blocking).
     */
    success<error> start();

    /**
     * Update internal driver state, blocks until it receives another command.
     * Thread-safe blocking.
     */
    success<error> update();

    /**
     * Copies the internal state. May thread-safe block.
     */
    state copy_state();

private:

    // nmea command processing
    success<error> process_gga(const char *str);
    success<error> process_rmc(const char *str);

    success<error> process_command(const char *str);

    state internal_state;
    mutex state_mutex;
    uart_buffered uart;
};

} // namespace sdk

#endif // AIRBRAKES_SDK_CDPA1616D_H_
