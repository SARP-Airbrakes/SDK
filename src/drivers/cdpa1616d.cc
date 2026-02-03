
#include <cmath>
#include <cstdio>
#include <sdk/drivers/cdpa1616d.h>
#include <sdk/scoped_lock.h>

namespace sdk {

static const uint8_t CMD_BAUD_38400[] = "$PMTK251,38400*27\r\n";
static const uint8_t CMD_UPDATE_10HZ[] = "$PMTK220,100*2F\r\n";
static const uint8_t CMD_FIXCTL_10HZ[] = "$PMTK500,100,0,0,0.0,0.0*2A\r\n";

success<cdpa1616d::error> cdpa1616d::start()
{
    uart.transmit(CMD_BAUD_38400, sizeof(CMD_BAUD_38400) - 1);
    uart.set_baud(38400);
    uart.transmit(CMD_UPDATE_10HZ, sizeof(CMD_UPDATE_10HZ) - 1);
    uart.transmit(CMD_FIXCTL_10HZ, sizeof(CMD_FIXCTL_10HZ) - 1);

    RESULT_UNWRAP_OR(uart.read(), error::UART);
    return success<error>();
}

success<cdpa1616d::error> cdpa1616d::update()
{
    uint8_t buffer[128];

    // commands end with \r\n
    auto size = uart.next('\n'); 
    RESULT_UNWRAP_OR(size, error::UART);

    // leave space for null byte
    size_t to_read = size.unwrap() < (sizeof(buffer) - 1) ? size.unwrap() :
        (sizeof(buffer) - 1);

    RESULT_UNWRAP_OR(uart.move(buffer, to_read), error::UART);
    buffer[to_read] = 0;

    // unsafe cast
    RESULT_UNWRAP(process_command((const char *) buffer));
    
    return success<error>();
}

cdpa1616d::state cdpa1616d::copy_state()
{
    // lock is dropped at return
    scoped_lock lock(state_mutex);
    return internal_state;
}

success<cdpa1616d::error> cdpa1616d::process_command(
    const char *str)
{
    switch (str[3]) {
    case 'G': // GPGGA, GPGSA, GPGSV
        switch (str[4]) {
        case 'G': // GPGGA
            int num;
            float utc;
            float latitude;
            char ns_indicator;
            float longitude;
            char ew_indicator;

            utc = NAN;
            latitude = NAN;
            ns_indicator = 0;
            longitude = NAN;
            ew_indicator = 0;

            num = sscanf(
                &str[3], // skip first "$GN"
                "GGA,%f,%f,%c,%f,%c",
                &utc,
                &latitude,
                &ns_indicator,
                &longitude,
                &ew_indicator
            );
            return success<error>();
        case 'S': // GPGSA, GPGSV
            switch (str[5]) {
            case 'A': // GPGSA
                /* ignored */
                return success<error>();
            case 'V': // GPGSV
                /* ignored */
                return success<error>();
            }
            return error::INVALID_COMMAND;
        }
        return error::INVALID_COMMAND;
    case 'R': // GPRMC
        return success<error>();
    case 'V': // GPVTG
        /* ignored */
        return success<error>();
    }
    return error::INVALID_COMMAND;
}

} // namespace sdk
