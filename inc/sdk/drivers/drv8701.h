
#ifndef AIRBRAKES_SDK_DRV8701_H_
#define AIRBRAKES_SDK_DRV8701_H_

#include <sdk/pwm.h>
#include <sdk/unique_pin.h>

#include <utility>

namespace sdk {

/**
 * Class representing the interface for the DRV8701 motor driver.
 */
class drv8701 {
public:
    drv8701(
        real epsilon,
        pwm &&in1, 
        pwm &&in2
    ) : epsilon(epsilon), in1(std::move(in1)), in2(std::move(in2))
    {
    }

    void start();
    void stop();

    /** Takes a power level [-1,1] */
    void set_power(real power);

private:
    real epsilon;
    pwm in1;
    pwm in2;
};

} // namespace sdk

#endif // AIRBRAKES_SDK_DRV8701_H_
