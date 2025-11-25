
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
    uint8_t cmd[4];
    cmd[0] = PAGE_PROGRAM_COMMAND;
    cmd[1] = (address >> 16) & 0xFF; // msb
    cmd[2] = (address >> 8) & 0xFF;
    cmd[3] = address & 0xFF; // lsb

    enable_write();
    enable_chip();

    // first transmit command
    interface.transmit(cmd, sizeof(cmd));

    // then write data to chip
    interface.transmit(data, data_size);

    disable_chip();
}

void w25q16jv::enable_write()
{
    uint8_t cmd[1];
    cmd[0] = WRITE_ENABLE_COMMAND;

    enable_chip();

    // transmit command
    interface.transmit(cmd, sizeof(cmd));

    disable_chip();
}

void w25q16jv::enable_chip()
{
    pin.write(0); // drive cs low
}

void w25q16jv::disable_chip()
{
    pin.write(1); // drive cs high
}

} // namespace sdk
