
#include <sdk/i2c.h>
#include <sdk/scoped_lock.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

namespace sdk {

i2c_master::status i2c_master::read(uint16_t device_address, uint16_t
        reg_address, uint8_t *data, uint16_t data_size, bool mem_16bit) 
{ 
    // make sure the address can fit
    if (!mem_16bit)
        reg_address &= 0xff;
    
    // lock the interface mutex before read
    // scoped_lock lock(interface_mutex);

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read_IT(
        handle,
        device_address,
        reg_address,
        mem_16bit ? I2C_MEMADD_SIZE_16BIT : I2C_MEMADD_SIZE_8BIT,
        data,
        data_size
    );
    return status == HAL_OK ? status::OK : status::ERROR;
}

i2c_master::status i2c_master::write(uint16_t device_address, uint16_t
        reg_address, uint8_t *data, uint16_t data_size, bool mem_16bit)
{
    // make sure the address can fit
    if (!mem_16bit)
        reg_address &= 0xff;

    // lock the interface mutex before write
    // scoped_lock lock(interface_mutex);

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write(
        handle,
        device_address,
        reg_address,
        mem_16bit ? I2C_MEMADD_SIZE_16BIT : I2C_MEMADD_SIZE_8BIT,
        data,
        data_size,
        HAL_MAX_DELAY
    );

    return status == HAL_OK ? status::OK : status::ERROR;
}

} // namespace sdk
