
#ifndef AIRBRAKES_SDK_SCOPED_PIN_H_
#define AIRBRAKES_SDK_SCOPED_PIN_H_

#include <sdk/unique_pin.h>

namespace sdk {

/**
 * Represents a pin that is turned to a specific state for a scope, and then
 * reverted to some other state. Useful for chip select pins (during the scope,
 * the pin state should be false, meaning low, and then when the scope ends the
 * pin state should be true, meaning high).
 */
class scoped_pin {
public:
    
    explicit scoped_pin(unique_pin &pin, bool during = true, bool after = false) : pin(pin), after(after)
    {
        pin.write(during);
    }

    ~scoped_pin()
    {
        pin.write(after);
    }

private:
    unique_pin &pin;
    bool after;
};

} // namespace sdk

#endif // AIRBRAKES_SDK_SCOPED_PIN_H_
