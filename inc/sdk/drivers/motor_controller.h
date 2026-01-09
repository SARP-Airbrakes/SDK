
#ifndef AIRBRAKES_SDK_MOTOR_CONTROLLER_H_
#define AIRBRAKES_SDK_MOTOR_CONTROLLER_H_

#include <sdk/drivers/motor.h>
#include <sdk/drivers/quad_encoder.h>

#include <utility>

namespace sdk {

/**
 * Wrangles the motor driver and the encoder to try and achieve a specific angle
 * on the motor with a PID.
 */
class motor_controller {
public:
    motor_controller(
        float p, float i, float d,
        motor &&motor,
        quad_encoder &&encoder
    ) : p(p), i(i), d(d), target_motor(std::move(motor)),
            encoder(std::move(encoder))
    {
    }

    void set_target_degrees(float new_target);

    /** Recalculates motor power */
    void update_motor_power(float dt);

private:
    float target_degrees;
    float p, i, d;

    float integral_error;
    float last_error;

    motor target_motor;
    quad_encoder encoder;
    
};

} // namespace sdk

#endif // AIRBRAKES_SDK_MOTOR_CONTROLLER_H_
