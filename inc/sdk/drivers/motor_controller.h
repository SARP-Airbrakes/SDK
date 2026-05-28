
#ifndef AIRBRAKES_SDK_MOTOR_CONTROLLER_H_
#define AIRBRAKES_SDK_MOTOR_CONTROLLER_H_

#include <sdk/drivers/drv8701.h>
#include <sdk/drivers/quad_encoder.h>

#include <utility>
#include <optional>

namespace sdk {

/**
 * Wrangles the motor driver and the encoder to try and achieve a specific angle
 * on the motor with a PID.
 */
class motor_controller {
public:

    /* amount of degrees per second that constitutes a stall */
    static constexpr float MIN_STALL_DPS = 1;
    /* time in seconds where the motor has to stall for calibration to finish */
    static constexpr float MIN_CALIBRATION_STALL_DELTA = 0.2;

    enum class state {
        OFF,
        ACTIVE,
        CALIBRATING
    };
    
public:
    motor_controller(
        float p, float i, float d, int calibration_encoder_offset,
        drv8701 &&motor,
        quad_encoder &&encoder
    ) : p(p), i(i), d(d), target_motor(std::move(motor)),
            encoder(std::move(encoder)), calibration_encoder_offset(calibration_encoder_offset)
    {
    }

    void calibrate();
    void start();
    void stop();

    /** Sets target degrees */
    void set_target_degrees(float new_target);

    /** Recalculates motor power. Thread-safe blocking. */
    void update_motor(float dt);

    float target_degrees = 0;
    float commanded_power = 0;
    float p, i, d;

    float integral_error = 0;
    float last_error = 0;

    std::optional<float> last_encoder_degrees = 0;
    std::optional<float> calibration_hold_time = 0;

private:
    void switch_states(state new_state);

    void update_pid(float dt);
    void update_calibration(float dt);

public:
    drv8701 target_motor;
    quad_encoder encoder;

    int calibration_encoder_offset;

    state current_state;
    
};

} // namespace sdk

#endif // AIRBRAKES_SDK_MOTOR_CONTROLLER_H_
