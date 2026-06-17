
#include <sdk/drivers/quad_encoder.h>

#include "FreeRTOS.h"
#include "task.h"

namespace sdk {

static const int inc_dec_table[4][4] = {
    {0, -1, 1, 0},
    {1, 0, 0, -1},
    {-1, 0, 0, 1},
    {0, 1, -1, 0},
};

success<> quad_encoder::read_and_update(uint16_t updated_pin)
{
    int last_idx = (pin_a_value ? 2 : 0) | (pin_b_value ? 1 : 0);
    if (updated_pin == pin_a.get_pin())
        pin_a_value = pin_a.read();
    else if (updated_pin == pin_b.get_pin())
        pin_b_value = pin_b.read();
    int next_idx = (pin_a_value ? 2 : 0) | (pin_b_value ? 1 : 0);
    int inc_dec = inc_dec_table[last_idx][next_idx];
    if (inc_dec == 0) {
        /* TODO: handling driver error conditions */
        return success<>(result_err::FAIL);
    } else {
        /* TODO: imprecision from the rounding */

        UBaseType_t status = taskENTER_CRITICAL_FROM_ISR();
        count += inc_dec;
        taskEXIT_CRITICAL_FROM_ISR(status);
    }
    return success<>();
}

int32_t quad_encoder::get_count() const
{
    int32_t local_count;
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        taskENTER_CRITICAL();
        local_count = count;
        taskEXIT_CRITICAL();
    } else {
        local_count = count;
    }
    return local_count;
}

float quad_encoder::get_revolutions() const
{
    return (real) get_count() / counts_per_rev;
}

float quad_encoder::get_degrees() const
{
    return get_revolutions() * 360.0f;
}

void quad_encoder::set_count(int32_t count)
{
    if (xTaskGetSchedulerState() != taskSCHEDULER_NOT_STARTED) {
        taskENTER_CRITICAL();
        this->count = count;
        taskEXIT_CRITICAL();
    } else {
        this->count = count;
    }
}

} // namespace sdk
