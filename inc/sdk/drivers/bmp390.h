
#ifndef AIRBRAKES_SDK_BMP390_H_
#define AIRBRAKES_SDK_BMP390_H_

#include <sdk/i2c.h>
#include <sdk/mutex.h>

namespace sdk {

/**
 * Class representing the driver for the BMP390 barometric altimeter.
 */
class bmp390 {
public:
    /** Driver state */
    struct state {
        float pressure_bars;
    };
    
public:

    bmp390(i2c_master &i2c) : i2c(i2c)
    {
    }

    void update();

    state copy_state();

private:
    state fetch_data();

    i2c_master &i2c;
    mutex state_mutex;
    state current_state;

};

} // namespace sdk


#endif // AIRBRAKES_SDK_BMP390_H_
