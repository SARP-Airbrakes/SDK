
#include <sdk/drivers/bmi088.h>

#include <sdk/result.h>
#include <sdk/scoped_lock.h>

namespace sdk {

success<bmi088::error> bmi088::set_acc_config(acc_range range, acc_bwp bwp, acc_odr odr)
{
    acc_range curr_range;
    acc_bwp curr_bwp;
    acc_odr curr_odr;

    // scoped read from internal state
    {
        scoped_lock lock(state_mutex);
        curr_range = internal_state.acc_range;
        curr_bwp = internal_state.acc_bwp;
        curr_odr = internal_state.acc_odr;
    }

    if (range != curr_range) { 
        auto status = i2c.write(
            SLAVE_ADDRESS_ACC << 1,
            ACC_RANGE_ADDR,
            (uint8_t *) &range,
            1,
            false
        );
        RESULT_UNWRAP_OR(status, error::I2C);
        scoped_lock lock(state_mutex);
        internal_state.acc_range = range;
    }

    if (bwp != curr_bwp || odr != curr_odr) {
        uint8_t acc_conf = 0;
        acc_conf |= (uint8_t)odr;
        acc_conf |= ((uint8_t)bwp) << 4;
        acc_conf |= 1 << 7; /* last bit must always be 1 (see 5.3.8) */
        auto status = i2c.write(
            SLAVE_ADDRESS_ACC << 1,
            ACC_CONF_ADDR,
            &acc_conf,
            1,
            false
        );
        RESULT_UNWRAP_OR(status, error::I2C);
        scoped_lock lock(state_mutex);
        internal_state.acc_bwp = bwp;
        internal_state.acc_odr = odr;
    }
    return success<error>();
}

success<bmi088::error> bmi088::set_gyro_config(gyro_range range, gyro_bw bw)
{
    gyro_range curr_range;
    gyro_bw curr_bw;

    {
        scoped_lock lock(state_mutex);
        curr_range = internal_state.gyro_range;
        curr_bw = internal_state.gyro_bw;
    }
    
    if (range != curr_range) {
        auto status = i2c.write(
            SLAVE_ADDRESS_GYRO << 1,
            GYRO_RANGE_ADDR,
            (uint8_t *) &range,
            1,
            false
        );
        RESULT_UNWRAP_OR(status, error::I2C);
        scoped_lock lock(state_mutex);
        internal_state.gyro_range = range;
    }

    if (bw != curr_bw) {
        auto status = i2c.write(
            SLAVE_ADDRESS_GYRO << 1,
            GYRO_BANDWIDTH_ADDR,
            (uint8_t *) &bw,
            1,
            false
        );
        RESULT_UNWRAP_OR(status, error::I2C);
        scoped_lock lock(state_mutex);
        internal_state.gyro_bw = bw;
    }
    return success<error>();
}

result<bool, bmi088::error> bmi088::is_connected()
{
    // just check for the acc
    uint8_t acc_chip_id = 0;
    auto status = i2c.read(
        SLAVE_ADDRESS_ACC << 1,
        ACC_CHIP_ID,
        &acc_chip_id,
        1,
        false
    );
    RESULT_UNWRAP_OR(status, error::I2C);
    return acc_chip_id == ACC_CHIP_ID;
}

success<bmi088::error> bmi088::update()
{
    state out;

    // first, copy the state (for stuff like last_sensortime, etc)
    {
        scoped_lock lock(state_mutex);
        out = internal_state;
    }

    // fetch relevant data
    RESULT_UNWRAP(fetch_data(out));
    
    // then copy it back
    scoped_lock lock(state_mutex);
    internal_state = out;

    return success<error>();
}

bmi088::state bmi088::copy_state()
{
    // lock dropped at return
    scoped_lock lock(state_mutex);
    return internal_state;
}

bmi088::real bmi088::sensortime_to_s(uint32_t sensortime)
{
    return SENSORTIME_RESOLUTION * (real)sensortime;
}

bmi088::real bmi088::get_delta_t(uint32_t last_sensortime, uint32_t sensortime)
{
    // overflow has occurred
    if (sensortime < last_sensortime)
        sensortime += 0x00ffffff;
    real out = sensortime_to_s(sensortime) - sensortime_to_s(last_sensortime);

    // really make sure that there is no negative times
    if (out < 0)
        return 0;
    return out;
}

static bmi088::real get_acc_range_multiplier(bmi088::acc_range range)
{
    switch (range) {
    case bmi088::acc_range::RANGE_3G:
        return 3.0f;
    case bmi088::acc_range::RANGE_6G:
        return 6.0f;
    case bmi088::acc_range::RANGE_12G:
        return 12.0f;
    case bmi088::acc_range::RANGE_24G:
        return 24.0f;
    }
}


success<bmi088::error> bmi088::fetch_acc_data(state &out)
{
    uint8_t data_frame[9];
    auto status = i2c.read(
        SLAVE_ADDRESS_ACC << 1,
        ACC_X_LSB_ADDR,
        data_frame,
        sizeof(data_frame),
        false
    );
    RESULT_UNWRAP_OR(status, error::I2C);
    int16_t accel_x = (data_frame[1] << 8) | data_frame[0];
    int16_t accel_y = (data_frame[3] << 8) | data_frame[2];
    int16_t accel_z = (data_frame[5] << 8) | data_frame[4];
    uint32_t sensortime = (data_frame[8] << 16) | (data_frame[7] << 8) |
        data_frame[6];

    real mult = get_acc_range_multiplier(out.acc_range);
    
    /* see 5.3.4 */
    out.acceleration_ms2.x = (GRAVITY_EARTH * (real)accel_x * mult) / 32768.0f;
    out.acceleration_ms2.y = (GRAVITY_EARTH * (real)accel_y * mult) / 32768.0f;
    out.acceleration_ms2.z = (GRAVITY_EARTH * (real)accel_z * mult) / 32768.0f;

    // its better to have a delta-T of 0 rather than a large amount
    out.last_sensortime = out.uninitialized_sensortime ? sensortime :
        out.sensortime;
    out.uninitialized_sensortime = false;
    out.sensortime = sensortime;
    return success<error>();
}

static bmi088::real get_gyro_range_multiplier(bmi088::gyro_range range)
{
    switch (range) {
    case bmi088::gyro_range::RANGE_2000DPS:
        return 2000.0f;
    case bmi088::gyro_range::RANGE_1000DPS:
        return 1000.0f;
    case bmi088::gyro_range::RANGE_500DPS:
        return 500.0f;
    case bmi088::gyro_range::RANGE_250DPS:
        return 250.0f;
    case bmi088::gyro_range::RANGE_125DPS:
        return 125.0f;
    }
}

success<bmi088::error> bmi088::fetch_gyro_data(state &out)
{
    uint8_t data_frame[6];
    auto status = i2c.read(
        SLAVE_ADDRESS_GYRO << 1,
        RATE_X_LSB_ADDR,
        data_frame,
        sizeof(data_frame),
        false
    );
    RESULT_UNWRAP_OR(status, error::I2C);
    int16_t rate_x = (data_frame[1] << 8) | data_frame[0];
    int16_t rate_y = (data_frame[3] << 8) | data_frame[2];
    int16_t rate_z = (data_frame[5] << 8) | data_frame[4];

    real mult = get_gyro_range_multiplier(out.gyro_range);
    out.angular_velocity_ds.x = (rate_x * mult) / 32768.0f;
    out.angular_velocity_ds.y = (rate_y * mult) / 32768.0f;
    out.angular_velocity_ds.z = (rate_z * mult) / 32768.0f;

    real delta_t = get_delta_t(out.last_sensortime, out.sensortime);
    out.orientation_deg.x += out.angular_velocity_ds.x * delta_t;
    out.orientation_deg.y += out.angular_velocity_ds.y * delta_t;
    out.orientation_deg.z += out.angular_velocity_ds.z * delta_t;
    return success<error>();
}

success<bmi088::error> bmi088::fetch_data(state &out)
{
    RESULT_UNWRAP_OR(fetch_acc_data(out), bmi088::error::ACC);
    RESULT_UNWRAP_OR(fetch_gyro_data(out), bmi088::error::GYRO);
    return success<error>();
}

} // namespace sdk
