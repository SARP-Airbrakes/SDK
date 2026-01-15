
#include <sdk/drivers/bmp390.h>

#include <sdk/scoped_lock.h>

namespace sdk {

void bmp390::read_calibration_data()
{
    uint8_t reg_data[21];
    i2c_master::status status = i2c.read(
        SLAVE_ADDRESS << 1,
        NVM_PAR_T1_ADDR,
        reg_data,
        sizeof(reg_data),
        false
    );

    if (status != i2c_master::status::OK) {
        /* TODO: error condition */
        return;
    }

    /* this is derived from boschsensortec/BMP3_SensorAPI */
    uint16_t p = (reg_data[1] << 8) | reg_data[0];
    calib_data.par_t1 = ((real)p * (real)(1 << 8));
    p = (reg_data[3] << 8) | reg_data[2];
    calib_data.par_t2 = ((real)p / (real)(1 << 30));
    p = reg_data[4];
    calib_data.par_t3 = ((real)p / (real)((uint64_t) 1 << 48));
    p = (reg_data[6] << 8) | reg_data[5];
    calib_data.par_p1 = ((real)(p - 16384) / (real)(1 << 20));
    p = (reg_data[8] << 8) | reg_data[7];
    calib_data.par_p2 = ((real)(p - 16384) / (real)(1 << 29)); 
    p = reg_data[9];
    calib_data.par_p3 = ((real)p / (real)((uint64_t) 1 << 32));
    p = reg_data[10];
    calib_data.par_p4 = ((real)p / (real)((uint64_t) 1 << 37));
    p = (reg_data[12] << 8) | reg_data[11];
    calib_data.par_p5 = ((real)p * (real)(1 << 3));
    p = (reg_data[14] << 8) | reg_data[13];
    calib_data.par_p6 = ((real)p / (real)(1 << 6));
    p = reg_data[15];
    calib_data.par_p7 = ((real)p / (real)(1 << 8));
    p = reg_data[16];
    calib_data.par_p8 = ((real)p / (real)(1 << 15));
    p = (reg_data[18] << 8) | reg_data[17];
    calib_data.par_p9 = ((real)p / (real)((uint64_t) 1 << 48));
    p = reg_data[19];
    calib_data.par_p10 = ((real)p / (real)((uint64_t) 1 << 48));
    p = reg_data[20];
    /* 2^65 */
    calib_data.par_p11 = ((real)p / 36893488147419103232.0f);
}

void bmp390::update()
{
    state out;
    bool success = fetch_data(out);
    if (!success) {
        /* TODO: error condition */
        return;
    }

    scoped_lock lock(state_mutex);
    current_state = out;
}

void bmp390::set_config(uint8_t filter_coefficient)
{
    uint8_t config = (filter_coefficient & 0x07) << 1;
    i2c.write(SLAVE_ADDRESS << 1, CONFIG_ADDR, &config, sizeof(config), false);
    /* TODO: error handling */
}

bmp390::state bmp390::copy_state()
{
    // scope is dropped on return
    scoped_lock lock(state_mutex);
    return current_state;
}

bmp390::real bmp390::compensate_temperature(data_frame frame)
{
    uint32_t uncomp = 0;
    uncomp |= frame[0];
    uncomp |= frame[1] << 8;
    uncomp |= frame[2] << 16;
    real partial0 = (real)uncomp - calib_data.par_t1;
    real partial1 = partial0 * calib_data.par_t2;

    // TODO: precision checks (see BMP3_SensorAPI, bmp3.c lines 2506-2516)
    return partial1 + (partial0 * partial0) * calib_data.par_t3;
}

bmp390::real bmp390::compensate_pressure(real temp_c, data_frame frame)
{
    uint32_t uncomp = 0;
    uncomp |= frame[3];
    uncomp |= frame[4] << 8;
    uncomp |= frame[5] << 16;

    real partial0 = calib_data.par_p6 * temp_c;
    real partial1 = calib_data.par_p7 * temp_c * temp_c;
    real partial2 = calib_data.par_p8 * temp_c * temp_c * temp_c;
    real partial_out0 = calib_data.par_p5 + partial0 + partial1 + partial2;

    partial0 = calib_data.par_p2 * temp_c;
    partial1 = calib_data.par_p3 * temp_c * temp_c;
    partial2 = calib_data.par_p4 * temp_c * temp_c * temp_c;
    real partial_out1 = uncomp * 
        (calib_data.par_p1 + partial0 + partial1 + partial2);

    partial0 = uncomp * uncomp;
    partial1 = calib_data.par_p9 + calib_data.par_p10 * temp_c;
    partial2 = partial0 * partial1 + 
        uncomp * uncomp * uncomp * calib_data.par_p11;
    
    real out = partial_out0 + partial_out1 + partial2;

    // TODO: precision checks (see BMP3_SensorAPI, bmp3.c lines 2506-2516)
    return out;
}

bool bmp390::fetch_data(state &out)
{
    // also reads reversed bytes
    data_frame frame;
    if (i2c.read(
        SLAVE_ADDRESS << 1,
        DATA_0_ADDR,
        frame,
        sizeof(data_frame),
        false
    ) != i2c_master::status::OK) {
        /* TODO: error condition */
        return false;
    };
    out.temperature_celsius = compensate_temperature(frame);
    out.pressure_pascals = compensate_pressure(out.temperature_celsius, frame);
    return true;
}

} // namespace sdk


