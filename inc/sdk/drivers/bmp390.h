
#ifndef AIRBRAKES_SDK_BMP390_H_
#define AIRBRAKES_SDK_BMP390_H_

#include <sdk/i2c.h>
#include <sdk/mutex.h>

namespace sdk {

/**
 * Class representing the driver for the BMP390 barometric altimeter.
 */
class bmp390 {
public:
    static constexpr int SLAVE_ADDRESS = 0x76;
    static constexpr int CHIP_ID_FIXED = 0x60;

    static constexpr int CHIP_ID_ADDR = 0x00;
    static constexpr int DATA_0_ADDR = 0x04;
    static constexpr int CONFIG_ADDR = 0x1F;
    static constexpr int NVM_PAR_T1_ADDR = 0x31;

    /* this might be too low precision */
    using real = float;
    using data_frame = uint8_t[8];

    enum class error {
        OK,
        I2C
    };

    /** Driver state */
    struct state {
        real temperature_celsius;
        real pressure_pascals;
    };
    
public:

    bmp390(i2c_master &i2c) : i2c(i2c)
    {
    }

    /** Gets if this chip is connected. Thread-safe blocking. */
    result<bool, error> is_connected();

    /**
     * Reads the calibration data from the chip. Thread-safe blocking.
     */
    success<error> read_calibration_data();

    /**
     * Updates internal driver state with new data received from the chip.
     * Thread-safe blocking.
     */
    success<error> update();

    /**
     * Sets the CONFIG register with the given filter coefficient value (see
     * 4.3.21). Thread-safe blocking.
     */
    success<error> set_config(uint8_t filter_coefficient);

    state copy_state(); /* may thread-safe block */

private:
    /* (see 3.11.1) */
    struct calibration {
        /* temperature calibration values */
        real par_t1;
        real par_t2;
        real par_t3;

        /* pressure calibration values */
        real par_p1;
        real par_p2;
        real par_p3;
        real par_p4;
        real par_p5;
        real par_p6;
        real par_p7;
        real par_p8;
        real par_p9;
        real par_p10;
        real par_p11;
    };

    calibration calib_data;

private:
    /* gets the temperature from a data frame (in degrees celsius) */
    real compensate_temperature(data_frame frame);
    /* gets the pressure from a data frame (in pascals) */
    real compensate_pressure(real temperature_celsius, data_frame frame);
    
    result<state, error> fetch_data();

    i2c_master &i2c;
    mutex state_mutex;
    state current_state;

};

} // namespace sdk


#endif // AIRBRAKES_SDK_BMP390_H_
