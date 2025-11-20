
#include <sdk/drivers/w25q16jv.h>

#include <cstring>

namespace sdk {

void w25q16jv::update()
{
    while (!write_queue.is_empty()) {
        write_command command = write_queue.pop();

        vPortFree(command.data);
    }
}

void w25q16jv::queue_write(uint32_t address, const uint8_t *data, uint32_t data_size)
{
    // copy data before sending it to the driver thread
    uint8_t *data_copy = (uint8_t *) pvPortMalloc(data_size);
    memcpy(data_copy, data, data_size);

    // send it off to the driver thread
    write_command command{address, data_copy, data_size};
    write_queue.push_back(command);
}

void w25q16jv::write(uint32_t address, uint8_t *data, uint32_t data_size)
{
}

} // namespace sdk
