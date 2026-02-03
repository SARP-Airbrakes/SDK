
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

    using real = pwm::real;

public:

    drv8701(
        real epsilon,
        pwm &&in1, 
        pwm &&in2, 
        unique_pin &&sh1, 
        unique_pin &&sh2,
        unique_pin &&nsleep
    ) : epsilon(epsilon), in1(std::move(in1)), in2(std::move(in2)),
            sh1(std::move(sh1)), sh2(std::move(sh2)), nsleep(std::move(nsleep))
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
    unique_pin sh1;
    unique_pin sh2;
    unique_pin nsleep;
};

} // namespace sdk

#endif // AIRBRAKES_SDK_DRV8701_H_
