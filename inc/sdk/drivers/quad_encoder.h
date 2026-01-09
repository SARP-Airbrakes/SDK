
#ifndef AIRBRAKES_SDK_QUAD_ENCODER_H_
#define AIRBRAKES_SDK_QUAD_ENCODER_H_

#include <sdk/unique_pin.h>
#include <stdint.h>
#include <utility>

namespace sdk {

/**
 * Class representing the interface for the quad. encoder.
 */
class quad_encoder {
public:

    quad_encoder(
        float counts_per_rev, // encoder counts per revolution of motor shaft
        unique_pin &&pin_a,
        unique_pin &&pin_b
    ) : counts_per_rev(counts_per_rev), pin_a(std::move(pin_a)),
            pin_b(std::move(pin_b))
    {
    }

    /** Reads current pin states and updates internal driver state */
    void read_and_update(uint16_t updated_pin);

    /** Returns the latest value read from the quad. encoder */
    float get_revolutions();
    float get_degrees();

private:

    int count;
    float counts_per_rev;

    unique_pin pin_a, pin_b;
    bool pin_a_value, pin_b_value;
};

} // namespace sdk

#endif // AIRBRAKES_SDK_QUAD_ENCODER_H_
