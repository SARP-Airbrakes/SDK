
#ifndef AIRBRAKES_SDK_BMI088_H_
#define AIRBRAKES_SDK_BMI088_H_

#include <sdk/i2c.h>
#include <sdk/mutex.h>

namespace sdk {

/**
 * This class represents the driver for the BMI088 inertial measurement unit
 * (IMU).
 */
class bmi088 {
public:
    static constexpr int SLAVE_ADDRESS_ACC = 0x18;
    static constexpr int SLAVE_ADDRESS_GYRO = 0x68;

    static constexpr int ACC_CHIP_ID = 0x1E;
    static constexpr int ACC_CHIP_ID_ADDR = 0x00;
    static constexpr int ACC_X_LSB_ADDR = 0x12;

    static constexpr float GRAVITY_EARTH = 9.80665f;

    enum class acc_range : uint8_t {
        RANGE_3G = 0x00,
        RANGE_6G = 0x01,
        RANGE_12G = 0x02,
        RANGE_24G = 0x03,
    };

public:
    using real = float;

    struct vec3 {
        real x, y, z;
    };
    
    struct state {
        vec3 acceleration_ms2;
        vec3 orientation_deg;
    };

public:

    bmi088(sdk::i2c_master &i2c) : i2c(i2c)
    {
    }

    /** Gets if this chip is connected. Thread-safe blocking. */
    bool is_connected();

    /**
     * Updates internal driver state. Thread-safe blocking.
     */
    void update();

    /**
     * Copies the internal driver state for use in a control loop. May thread-safe
     * block if it is still copying data into the driver.
     */
    state copy_state();

private:
    bool fetch_acc_data(state &out);
    bool fetch_data(state &out);

    sdk::i2c_master &i2c;
    acc_range curr_range;
    state internal_state;
    mutex state_mutex;   
};

} // namespace sdk

#endif // AIRBRAKES_SDK_BMI088_H_

