
#ifndef AIRBRAKES_SDK_QUAD_ENCODER_H_
#define AIRBRAKES_SDK_QUAD_ENCODER_H_

#include <sdk/unique_pin.h>
#include <sdk/result.h>
#include <stdint.h>
#include <utility>

namespace sdk {

/**
 * Class representing the interface for the quad. encoder.
 */
class quad_encoder {
public:

    using real = float;

public:

    quad_encoder(
        real counts_per_rev, // encoder counts per revolution of motor shaft
        unique_pin &&pin_a,
        unique_pin &&pin_b
    ) : counts_per_rev(counts_per_rev), pin_a(std::move(pin_a)),
            pin_b(std::move(pin_b)), count(0)
    {
        pin_a_value = pin_a.read();
        pin_b_value = pin_b.read();
    }

    /** Reads current pin states and updates internal driver state */
    success<> read_and_update(uint16_t updated_pin);

    /** Returns the latest value read from the quad. encoder */
    int32_t get_count() const;
    real get_revolutions() const;
    real get_degrees() const;

    void set_count(int32_t count);

private:

    real counts_per_rev;

    unique_pin pin_a, pin_b;

    volatile int32_t count;
    volatile bool pin_a_value, pin_b_value;
};

} // namespace sdk

#endif // AIRBRAKES_SDK_QUAD_ENCODER_H_
