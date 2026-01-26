
#include <sdk/i2c.h>
#include <sdk/scoped_lock.h>

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"

namespace sdk {

i2c_master *i2c_master::from_handle(I2C_HandleTypeDef *handle)
{
    return (i2c_master *) handle->hdmatx;
}

success<i2c_master::error> i2c_master::read(uint16_t device_address, uint16_t
        reg_address, uint8_t *data, uint16_t data_size, bool mem_16bit) 
{ 
    // make sure the address can fit
    if (!mem_16bit)
        reg_address &= 0xff;
    
    // lock the interface mutex before read
    scoped_lock lock(interface_mutex);

    HAL_StatusTypeDef status = HAL_I2C_Mem_Read_IT(
        handle,
        device_address,
        reg_address,
        mem_16bit ? I2C_MEMADD_SIZE_16BIT : I2C_MEMADD_SIZE_8BIT,
        data,
        data_size
    );

    blocked_task = xTaskGetCurrentTaskHandle();
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    return success<error>(status == HAL_OK ? error::OK : error::ERROR);
}

success<i2c_master::error> i2c_master::write(uint16_t device_address, uint16_t
        reg_address, uint8_t *data, uint16_t data_size, bool mem_16bit)
{
    // make sure the address can fit
    if (!mem_16bit)
        reg_address &= 0xff;

    // lock the interface mutex before write
    scoped_lock lock(interface_mutex);

    HAL_StatusTypeDef status = HAL_I2C_Mem_Write_IT(
        handle,
        device_address,
        reg_address,
        mem_16bit ? I2C_MEMADD_SIZE_16BIT : I2C_MEMADD_SIZE_8BIT,
        data,
        data_size
    );

    blocked_task = xTaskGetCurrentTaskHandle();
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    return success<error>(status == HAL_OK ? error::OK : error::ERROR);
}

void i2c_master::unblock_from_isr()
{
    if (blocked_task == nullptr) {
        /* TODO: this is an error condition! */
        return;
    }
    BaseType_t task_woken;
    vTaskNotifyGiveFromISR(blocked_task, &task_woken);
    blocked_task = nullptr;
    portYIELD_FROM_ISR(task_woken);
}

} // namespace sdk
