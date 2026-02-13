
#ifndef AIRBRAKES_SDK_BMI088_H_
#define AIRBRAKES_SDK_BMI088_H_

#include <sdk/i2c.h>
#include <sdk/mutex.h>
#include <sdk/result.h>

namespace sdk {

/**
 * This class represents the driver for the BMI088 inertial measurement unit
 * (IMU).
 */
class bmi088 {
public:
    static constexpr int SLAVE_ADDRESS_ACC = 0x18;
    static constexpr int SLAVE_ADDRESS_GYRO = 0x68;

    static constexpr int ACC_CHIP_ID = 0x1e;

    static constexpr int ACC_CHIP_ID_ADDR = 0x00;
    static constexpr int ACC_X_LSB_ADDR = 0x12;
    static constexpr int ACC_CONF_ADDR = 0x40;
    static constexpr int ACC_RANGE_ADDR = 0x41;

    static constexpr int RATE_X_LSB_ADDR = 0x02;
    static constexpr int GYRO_RANGE_ADDR = 0x0f;
    static constexpr int GYRO_BANDWIDTH_ADDR = 0x10;

    /** Accelerometer low-pass filter bandwidth. see 4.4.1 and 5.3.8 */
    enum class acc_bwp : uint8_t {
        OSR4 = 0x00, /* 4-fold oversampling */
        OSR2 = 0x01, /* 2-fold oversamping */
        NORMAL = 0x02, /* no oversampling */
    };

    /** Accelerometer output data rate. See 5.3.8 */
    enum class acc_odr : uint8_t {
        ODR_12_5HZ = 0x05, /* 12.5 Hz */
        ODR_25HZ = 0x06, /* 25 Hz */
        ODR_50HZ = 0x07, /* 50 Hz */
        ODR_100HZ = 0x08, /* 100 Hz */
        ODR_200HZ = 0x09, /* 200 Hz */
        ODR_400HZ = 0x0a, /* 400 Hz */
        ODR_800HZ = 0x0b, /* 800 Hz */
        ODR_1600HZ = 0x0c, /* 1600 Hz */
    };

    /** Accelerometer range setting. See 5.3.9 */
    enum class acc_range : uint8_t {
        RANGE_3G = 0x00,
        RANGE_6G = 0x01,
        RANGE_12G = 0x02,
        RANGE_24G = 0x03,
    };

    /** Gyroscope range setting. See 5.5.4 */
    enum class gyro_range : uint8_t {
        RANGE_2000DPS = 0x00, /* 2000 deg/s */
        RANGE_1000DPS = 0x01, /* 1000 deg/s */
        RANGE_500DPS = 0x02, /* 500 deg/s */
        RANGE_250DPS = 0x03, /* 250 deg/s */
        RANGE_125DPS = 0x04, /* 125 deg/s */
    };

    /** Gyroscope output data rate and filter bandwidth setting. See 5.5.5  */
    enum class gyro_bw : uint8_t {
        BW_532HZ = 0x00, /* BW: 532Hz, ODR: 2000Hz */
        BW_230HZ = 0x01, /* BW: 230Hz, ODR: 2000Hz */
        BW_116HZ = 0x02, /* BW: 116Hz, ODR: 1000Hz */
        BW_47HZ = 0x03, /* BW: 47Hz, ODR: 400Hz */
        BW_23HZ = 0x04, /* BW: 23Hz, ODR: 200Hz */
        BW_12HZ = 0x05, /* BW: 12Hz, ODR: 100Hz */
        BW_64HZ = 0x06, /* BW: 64Hz, ODR: 200Hz */
        BW_32HZ = 0x07, /* BW: 32Hz, ODR: 100Hz */
    };

    enum class error {
        OK,
        I2C,
        ACC,
        GYRO,
    };

    using real = float;

    /* seconds per sensortime lsb */
    static constexpr real SENSORTIME_RESOLUTION = 1.0f / 256000.0f;
    static constexpr real GRAVITY_EARTH = 9.80665f; /* m/s^2 */

    struct vec3 {
        real x, y, z;
    };
    
    struct state {
        vec3 acceleration_ms2; /* in m/s^2 */
        vec3 orientation_deg; /* in deg */
        vec3 angular_velocity_ds; /* in deg/s */

        uint32_t last_sensortime;
        uint32_t sensortime;
        bool uninitialized_sensortime = true;

        acc_range acc_range = acc_range::RANGE_6G;
        acc_bwp acc_bwp = acc_bwp::NORMAL;
        acc_odr acc_odr = acc_odr::ODR_100HZ;

        gyro_range gyro_range = gyro_range::RANGE_2000DPS;
        gyro_bw gyro_bw = gyro_bw::BW_532HZ;
    };

public:

    bmi088(sdk::i2c_master &i2c) : i2c(i2c)
    {
    }

    /** Start the accelerometer and gyroscope. Thread-safe blocking. */
    void start();
    /** Stop the accelerometer and gyroscope. Thread-safe blocking. */
    void stop();

    /** Sets the configuration of the accelerometer. Thread-safe blocking. */
    success<error> set_acc_config(acc_range range, acc_bwp bwp, acc_odr odr);
    /** Sets the configuration of the gyroscope. Thread-safe blocking. */
    success<error> set_gyro_config(gyro_range range, gyro_bw bw);

    /** Gets if this chip is connected. Thread-safe blocking. */
    result<bool, error> is_connected();

    /**
     * Updates internal driver state. Thread-safe blocking.
     */
    success<error> update();

    /**
     * Copies the internal driver state for use in a control loop. May thread-safe
     * block if it is still copying data into the driver.
     */
    state copy_state();

private:
    real sensortime_to_s(uint32_t sensortime);

    /** Gets the difference (in s) between two sensortimes. */
    real get_delta_t(uint32_t last_sensortime, uint32_t sensortime);

    success<error> fetch_acc_data(state &out);
    success<error> fetch_gyro_data(state &out);
    success<error> fetch_data(state &out);

    sdk::i2c_master &i2c;

    state internal_state;
    mutex state_mutex;   
};

} // namespace sdk

#endif // AIRBRAKES_SDK_BMI088_H_

