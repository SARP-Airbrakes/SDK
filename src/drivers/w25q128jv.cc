
#include <sdk/drivers/w25q128jv.h>

#include <cstring>

namespace sdk {

success<w25q128jv::error> w25q128jv::read(uint32_t address,
        uint8_t *data, uint32_t data_size)
{
    uint8_t cmd[4];
    cmd[0] = READ_DATA_COMMAND;
    cmd[1] = (address >> 16) & 0xFF; // msb
    cmd[2] = (address >> 8) & 0xFF;
    cmd[3] = address & 0xFF; // lsb

    RESULT_UNWRAP(block_for_busy_bit());
    
    // pin enable scope
    {
        auto pin = enable_chip();

        // send command for read
        auto status = interface.transmit(cmd, sizeof(cmd));
        RESULT_UNWRAP_OR(status, error::SPI);

        // FIXME: there could be timing issues here? how long would it take to
        // get back to here (timing critical)

        // then read data from chip
        status = interface.receive(data, data_size);
        RESULT_UNWRAP_OR(status, error::SPI);
    }

    return success<error>();
}

success<w25q128jv::error> w25q128jv::write(uint32_t address,
        const uint8_t *data, uint32_t data_size)
{
    uint8_t cmd[4];
    cmd[0] = PAGE_PROGRAM_COMMAND;
    cmd[1] = (address >> 16) & 0xFF; // msb
    cmd[2] = (address >> 8) & 0xFF;
    cmd[3] = address & 0xFF; // lsb

    RESULT_UNWRAP(block_for_busy_bit());
    RESULT_UNWRAP(enable_write());

    // pin enable scope
    {
        auto pin = enable_chip();

        // first transmit command
        auto status = interface.transmit(cmd, sizeof(cmd));
        RESULT_UNWRAP_OR(status, error::SPI);

        // then write data to chip
        status = interface.transmit(data, data_size);
        RESULT_UNWRAP_OR(status, error::SPI);
    }

    return success<error>();
}

success<w25q128jv::error> w25q128jv::enable_write()
{
    uint8_t cmd[1];
    cmd[0] = WRITE_ENABLE_COMMAND;

    // pin enable scope
    {
        auto pin = enable_chip();

        // transmit command
        auto status = interface.transmit(cmd, sizeof(cmd));
        RESULT_UNWRAP_OR(status, error::SPI);
    }

    return success<error>();
}

scoped_pin w25q128jv::enable_chip()
{
    // pull to low, reset to high
    return scoped_pin(cs_pin, false, true);
}

success<w25q128jv::error> w25q128jv::block_for_busy_bit(uint8_t attempts)
{
    while (attempts-- > 0) {
        auto reg = read_status_register_1();
        RESULT_UNWRAP(reg);
        
        auto value = reg.unwrap();
        if ((value & 0x01) == 0)
            return success<error>();
    }
    return error::BUSY;
}

result<uint8_t, w25q128jv::error> w25q128jv::read_status_register_1()
{
    uint8_t cmd[1];
    cmd[0] = READ_STATUS_REGISTER_1_COMMAND;

    uint8_t out = 0;

    {
        auto pin = enable_chip();
        auto status = interface.transmit(cmd, sizeof(cmd));
        RESULT_UNWRAP_OR(status, error::SPI);

        status = interface.receive(&out, sizeof(out));
        RESULT_UNWRAP_OR(status, error::SPI);
    }
    return out;
}

} // namespace sdk
