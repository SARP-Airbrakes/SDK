
#include <sdk/drivers/drv8701.h>

namespace sdk {

void drv8701::start()
{
    in1.start();
    in2.start();
    nsleep.write(true);
}

void drv8701::stop()
{
    in1.stop();
    in2.stop();
    nsleep.write(false);
}

void drv8701::set_power(real power)
{
    if (power < -epsilon) {
        sh1.write(false);
        sh2.write(true);
        in1.set(0);
        in2.set(-power);
    } else if (power > epsilon) {
        sh1.write(true);
        sh2.write(false);
        in1.set(power);
        in2.set(0);
    } else { // assume brake
        sh1.write(false);
        sh2.write(false);
        in1.set(1);
        in2.set(1);
    }
}

} // namespace sdk
