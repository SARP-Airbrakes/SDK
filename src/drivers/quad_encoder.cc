
#include <sdk/drivers/quad_encoder.h>

namespace sdk {

static const int inc_dec_table[4][4] = {
    {0, -1, 1, 0},
    {1, 0, 0, -1},
    {-1, 0, 0, 1},
    {0, 1, -1, 0},
};

void quad_encoder::read_and_update(uint16_t updated_pin)
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
    } else {
        /* TODO: imprecision from the rounding */
        count += inc_dec;
    }
}

float quad_encoder::get_revolutions()
{
    return (float) count / counts_per_rev;
}

float quad_encoder::get_degrees()
{
    return get_revolutions() * 360.0f;
}

} // namespace sdk
