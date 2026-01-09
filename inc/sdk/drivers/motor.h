
#ifndef AIRBRAKES_SDK_MOTOR_H_
#define AIRBRAKES_SDK_MOTOR_H_

namespace sdk {

/**
 * Class representing the interface for the motor driver.
 */
class motor {
public:

    /** Takes a power level [-1,1] */
    float set_power(float power);
};

} // namespace sdk

#endif // AIRBRAKES_SDK_MOTOR_H_
