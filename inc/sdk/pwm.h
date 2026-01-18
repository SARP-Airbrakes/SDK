
#ifndef AIRBRAKES_SDK_PWM_H_
#define AIRBRAKES_SDK_PWM_H_

#include <stm32f4xx_hal.h>

namespace sdk {

class pwm {
public:
    enum class tim_channel : uint32_t {
        CHANNEL_1 = TIM_CHANNEL_1,
        CHANNEL_2 = TIM_CHANNEL_2,
        CHANNEL_3 = TIM_CHANNEL_3,
        CHANNEL_4 = TIM_CHANNEL_4
    };

    using real = float;

public:
    
    pwm(TIM_HandleTypeDef *htim, tim_channel channel) : htim(htim), channel(channel)
    {
    }

    void start();
    void stop();

    /** set pwm duty cycle. value is [0,1] */
    void set(real value);

    /** get value resolution (minimum step between values) */
    real get_resolution();

private:

    TIM_HandleTypeDef *htim;
    tim_channel channel;
};

} // namespace sdk

#endif // AIRBRAKES_SDK_PWM_H_
