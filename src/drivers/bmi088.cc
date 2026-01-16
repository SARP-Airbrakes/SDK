
#include <sdk/drivers/bmi088.h>

#include <sdk/scoped_lock.h>

namespace sdk {

bool bmi088::is_connected()
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
    if (status != i2c_master::status::OK) {
        return false;
    }
    return acc_chip_id == ACC_CHIP_ID;
}

void bmi088::update()
{
    state out;
    if (!fetch_data(out)) {
        return;
    }
    
    scoped_lock lock(state_mutex);
    internal_state = out;
}

bmi088::state bmi088::copy_state()
{
    // lock dropped at return
    scoped_lock lock(state_mutex);
    return internal_state;
}

bool bmi088::fetch_acc_data(state &out)
{
    uint8_t data_frame[6];
    auto status = i2c.read(
        SLAVE_ADDRESS_ACC << 1,
        ACC_X_LSB_ADDR,
        data_frame,
        sizeof(data_frame),
        false
    );
    if (status != i2c_master::status::OK) {
        return false;
    }
    uint16_t accel_x = (data_frame[1] << 8) | data_frame[0];
    uint16_t accel_y = (data_frame[3] << 8) | data_frame[2];
    uint16_t accel_z = (data_frame[5] << 8) | data_frame[4];
    
    /* see 5.3.4 */
}

bool bmi088::fetch_data(state &out)
{
    return fetch_acc_data(out);
}

} // namespace sdk
