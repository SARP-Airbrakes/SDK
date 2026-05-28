
#include <sdk/drivers/motor_controller.h>

#include <sdk/timing.h>

namespace sdk {

void motor_controller::calibrate()
{
    if (current_state == state::ACTIVE)
        switch_states(state::CALIBRATING);
}

void motor_controller::start()
{
    switch_states(state::ACTIVE);
    target_motor.start();
}

void motor_controller::stop()
{
    switch_states(state::OFF);
    target_motor.stop();
}

void motor_controller::set_target_degrees(float new_target)
{
    target_degrees = new_target;
}

void motor_controller::update_motor(float dt)
{
    switch (current_state) {
    case state::ACTIVE:
        update_pid(dt);
        break;
    case state::CALIBRATING:
        update_calibration(dt);
        break;
    default:
        break;
    }
}

void motor_controller::switch_states(state new_state)
{
    switch (current_state) {
    case state::ACTIVE:
        last_error = 0;
        integral_error = 0;
        break;
    case state::CALIBRATING:
        last_encoder_degrees = std::nullopt;
        calibration_hold_time = std::nullopt;
        break;
    default:
        break;
    }
    current_state = new_state;
}

void motor_controller::update_pid(float dt)
{
    float curr_degrees = encoder.get_degrees();
    float error = target_degrees - curr_degrees;

    integral_error += error * dt;

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

    commanded_power = output;

    target_motor.set_power(output);
}

void motor_controller::update_calibration(float dt)
{
    target_motor.set_power(-0.6f);
    if (!last_encoder_degrees) {
        last_encoder_degrees = encoder.get_degrees();
    } else {
        float current_encoder_degrees = encoder.get_degrees();
        float speed = (current_encoder_degrees - *last_encoder_degrees) / dt;
        last_encoder_degrees = current_encoder_degrees;
        if (speed <= MIN_STALL_DPS) {
            if (calibration_hold_time) {
                if (get_tick_seconds() - *calibration_hold_time >
                        MIN_CALIBRATION_STALL_DELTA) {

                    // temporary value
                    encoder.count = calibration_encoder_offset;
                    switch_states(state::ACTIVE);
                }
            } else {
                calibration_hold_time = get_tick_seconds();
            }
        } else {
            calibration_hold_time = std::nullopt;
        }
    }
}

} // namespace sdk
