
#include <sdk/drivers/drv8701.h>

namespace sdk {

void drv8701::start()
{
    in1.start();
    in2.start();
}

void drv8701::stop()
{
    in1.stop();
    in2.stop();
}

void drv8701::set_power(real power)
{
    if (power < -epsilon) {
        in1.set(0);
        in2.set(-power);
    } else if (power > epsilon) {
        in1.set(power);
        in2.set(0);
    } else { // assume brake
        in1.set(1);
        in2.set(1);
    }
}

} // namespace sdk
