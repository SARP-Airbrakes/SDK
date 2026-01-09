
#include <sdk/drivers/motor_controller.h>

namespace sdk {

void motor_controller::set_target_degrees(float new_target)
{
    target_degrees = new_target;
}

void motor_controller::update_motor_power(float dt)
{
    float curr_degrees = encoder.get_degrees();
    float error = curr_degrees - target_degrees;

    integral_error = error * dt;

    // reset integral error if error switches sign
    if ((last_error <= 0) != (error <= 0)) {
        integral_error = 0;
    }

    float p_term = -p * error;
    float i_term = -i * integral_error;
    float d_term = -d * (error - last_error) / dt;
    last_error = error;

    float output = p_term + i_term + d_term;

    // clamp the output value
    if (output > 1) output = 1;
    else if (output < -1) output = -1;

    target_motor.set_power(output);
}

} // namespace sdk
