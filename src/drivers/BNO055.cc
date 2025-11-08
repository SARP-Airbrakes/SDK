
#include <sdk/drivers/BNO055.h>

#include "stm32f4xx_hal.h"

namespace sdk {

bno055::bno055(i2c_master &master, uint8_t addr): master(master), addr(addr)
{
}

bno055::result bno055::is_connected()
{
    if (read_byte(CHIP_ID_ADDR) != CHIP_ID)
        return result::UNCONNECTED;
    set_page(0);
    return result::OK;
}

void bno055::set_page(int page)
{
    write_byte(PAGE_ID_ADDR, (uint8_t) page);
}

bool bno055::is_calibrated(sensor sensor)
{
    uint8_t calib_stat = read_byte(CALIB_STAT_ADDR);

    // see 4.3.54 
    switch (sensor) {
    case sensor::SYSTEM: // bits <7:6>
        return ((calib_stat >> 6) & 0x03) != 3;
    case sensor::GYROSCOPE: // bits <5:4>
        return ((calib_stat >> 4) & 0x03) != 3;
    case sensor::ACCELEROMETER: // bits <3:2>
        return ((calib_stat >> 2) & 0x03) != 3;
    case sensor::MAGNETOMETER: // bits <1:0>
        return (calib_stat & 0x03) != 3;
    }
}

void bno055::set_power_mode(pwr_mode mode)
{
    write_byte(PWR_MODE_ADDR, (uint8_t) mode);
}

bno055::pwr_mode bno055::get_power_mode()
{
    return (pwr_mode) read_byte(PWR_MODE_ADDR);
}

void bno055::set_operation_mode(opr_mode mode)
{
    write_byte(OPR_MODE_ADDR, (uint8_t) mode);
}

bno055::opr_mode bno055::get_operation_mode()
{
    return (opr_mode) read_byte(OPR_MODE_ADDR);
}

void bno055::suspend()
{
    set_power_mode(pwr_mode::SUSPEND);
}

uint8_t bno055::read_byte(uint8_t address)
{
    // TODO(fergus-xu): read i2c using hal wrapper
    return 0;
}

uint16_t bno055::read_short(uint8_t lsb, uint8_t msb)
{
    uint16_t out = 0;
    out |= read_byte(lsb);
    out |= ((uint16_t) read_byte(msb)) << 8;
    return out;
}

void bno055::write_byte(uint8_t address, uint8_t value)
{
    // TODO(fergus-xu): write i2c using hal wrapper
}

void bno055::delay(int ms)
{
    HAL_Delay(ms);
}

}
