
#ifndef AIRBRAKES_SDK_BMI088_H_
#define AIRBRAKES_SDK_BMI088_H_

#include <sdk/i2c.h>
#include <sdk/mutex.h>

namespace sdk {

class bmi088 {
public:
    using real = float;
    
    struct state {
        real acceleration_ms2;
        real orientation_x;
        real orientation_y;
        real orientation_z;
    };

public:

    bmi088(sdk::i2c_master &i2c) : i2c(i2c)
    {
    }

    /**
     * Updates internal driver state. Thread-safe blocking.
     */
    void update();

    /**
     * Copies the internal driver state for use in a control loop. May thread-safe
     * block if it is still copying data into the driver.
     */
    state copy_state();

private:
    bool fetch_data(state &out);

    sdk::i2c_master &i2c;
    state internal_state;
    mutex state_mutex;   
};

} // namespace sdk

#endif // AIRBRAKES_SDK_BMI088_H_

