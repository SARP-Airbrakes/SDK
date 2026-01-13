
#include <sdk/drivers/bmp390.h>
#include "sdk/scoped_lock.h"

namespace sdk {

void bmp390::update()
{
    state out = fetch_data();

    scoped_lock lock(state_mutex);
    current_state = out;
}

bmp390::state bmp390::copy_state()
{
    // scope is dropped on return
    scoped_lock lock(state_mutex);
    return current_state;
}

bmp390::state bmp390::fetch_data()
{
    state out;
    uint8_t data_frame[8];
}

} // namespace sdk


