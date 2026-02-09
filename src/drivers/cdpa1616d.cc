
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

static void process_utc(cdpa1616d::real utc, cdpa1616d::state &state)
{
    state.utc_hours = (uint8_t) utc / 10000;
    state.utc_minutes = (uint8_t) (utc - state.utc_hours * 10000) / 100;
    state.utc_seconds = utc - state.utc_hours * 10000 - state.utc_minutes * 100;
}

success<cdpa1616d::error> cdpa1616d::process_gga(const char *str)
{
    int num;
    real utc = NAN;
    real msl_altitude = 0;
    int fix_indicator = 0;

    // see Table 3
    num = sscanf(
        &str[3], // skip first "$GN"
        "GGA,%f,%*f,%*c,%*f,%*c,%d,%*d,%*f,%f",
        &utc,
        &fix_indicator,
        &msl_altitude
    );

    if (num == EOF)
        return error::INVALID_COMMAND;

    scoped_lock lock(state_mutex);
    process_utc(utc, internal_state);
    internal_state.altitude_meters = msl_altitude;

    if (fix_indicator == 1) {
        internal_state.fixed = gps_fix::GPS_FIX;
    } else if (fix_indicator == 2) {
        internal_state.fixed = gps_fix::DIFF_GPS_FIX;
    } else { // assume no fix
        internal_state.fixed = gps_fix::NO_FIX;
    }

    return success<error>();
}

success<cdpa1616d::error> cdpa1616d::process_rmc(const char *str)
{
    int num;
    real utc = NAN;
    char status = 0;
    real sog_knots = 0;
    real cog_degrees = 0;

    // see Table 11
    num = sscanf(
        &str[3], // skip first "$GN",
        "RMC,%f,%c,%*f,%*c,%*f,%*c,%f,%f",
        &utc,
        &status,
        &sog_knots,
        &cog_degrees
    );

    if (num == EOF)
        return error::INVALID_COMMAND;
    
    scoped_lock lock(state_mutex);
    process_utc(utc, internal_state);
    internal_state.data_valid = status == 'A';
    internal_state.speed_over_ground_knots = sog_knots;
    internal_state.course_over_ground_degrees = cog_degrees;
    return success<error>();
}

success<cdpa1616d::error> cdpa1616d::process_command(
    const char *str)
{
    switch (str[3]) {
    case 'G': // GPGGA, GPGSA, GPGSV
        switch (str[4]) {
        case 'G': // GPGGA
            return process_gga(str);
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
        return process_rmc(str);
    case 'V': // GPVTG
        /* ignored */
        return success<error>();
    }
    return error::INVALID_COMMAND;
}

} // namespace sdk
