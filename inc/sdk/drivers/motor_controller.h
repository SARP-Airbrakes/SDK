
#ifndef AIRBRAKES_SDK_MOTOR_CONTROLLER_H_
#define AIRBRAKES_SDK_MOTOR_CONTROLLER_H_

#include <sdk/drivers/drv8701.h>
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
        drv8701 &&motor,
        quad_encoder &&encoder
    ) : p(p), i(i), d(d), target_motor(std::move(motor)),
            encoder(std::move(encoder))
    {
    }

    void start();
    void stop();

    /** Sets target degrees */
    void set_target_degrees(float new_target);

    /** Recalculates motor power. Thread-safe blocking. */
    void update_motor(float dt);

private:
    float target_degrees;
    float p, i, d;

    float integral_error;
    float last_error;

    drv8701 target_motor;
    quad_encoder encoder;
    
};

} // namespace sdk

#endif // AIRBRAKES_SDK_MOTOR_CONTROLLER_H_
