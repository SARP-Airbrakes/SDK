
#include <sdk/drivers/w25q128jv.h>
#include <sdk/box.h>

#include <cmsis_os.h>

#include <cstdio>
#include <cstring>

namespace sdk {

result<bool, w25q128jv::error> w25q128jv::is_connected()
{
    uint8_t out_data[6] = { };
    uint8_t in_data[sizeof(out_data)] = { };
    out_data[0] = 0x90; // Read Device ID

    {
        auto pin = enable_chip();

        // read the device id
        auto status = interface.transmit_receive(out_data, in_data, sizeof(out_data));
        RESULT_UNWRAP_OR(status, error::SPI);
    }
    
    return in_data[4] == 0xef && in_data[5] == 0x17;
}

success<w25q128jv::error> w25q128jv::erase()
{
    uint8_t cmd[1];
    cmd[0] = CHIP_ERASE_COMMAND;

    RESULT_UNWRAP(block_for_busy_bit());
    RESULT_UNWRAP(enable_write());

    // pin enable scope
    {
        auto pin = enable_chip();

        // send command to erase
        auto status = interface.transmit(cmd, sizeof(cmd));
        RESULT_UNWRAP_OR(status, error::SPI);
    }
    
    // wait until we aren't busy
    RESULT_UNWRAP(block_for_busy_bit(4096));
    return success<error>();
}

success<w25q128jv::error> w25q128jv::read(uint32_t address,
        uint8_t *data, uint32_t data_size)
{
    uint32_t total_size = 4 + data_size;
    box tx_cmd_buf = box(total_size);
    box rx_cmd_buf = box(total_size);

    RESULT_UNWRAP_OR(tx_cmd_buf, error::TOO_BIG);
    RESULT_UNWRAP_OR(rx_cmd_buf, error::TOO_BIG);

    uint8_t *tx_buf_ptr = tx_cmd_buf.get<uint8_t>();
    uint8_t *rx_buf_ptr = rx_cmd_buf.get<uint8_t>();

    memset(tx_buf_ptr, 0, total_size);
    memset(rx_buf_ptr, 0, total_size);

    tx_buf_ptr[0] = READ_DATA_COMMAND;
    tx_buf_ptr[1] = (address >> 16) & 0xFF; // msb
    tx_buf_ptr[2] = (address >> 8) & 0xFF;
    tx_buf_ptr[3] = address & 0xFF; // lsb

    RESULT_UNWRAP(block_for_busy_bit());
    
    // pin enable scope
    {
        auto pin = enable_chip();
        auto status = interface.transmit_receive(tx_buf_ptr, rx_buf_ptr, total_size);
        RESULT_UNWRAP_OR(status, error::SPI);
    }

    memcpy(data, rx_buf_ptr + 4, data_size);

    return success<error>();
}

success<w25q128jv::error> w25q128jv::write(uint32_t address,
        const uint8_t *data, uint32_t data_size)
{
    uint32_t total_size = data_size + 4;

    box cmd_buf = box(total_size);
    RESULT_UNWRAP_OR(cmd_buf, error::TOO_BIG);

    uint8_t *ptr = cmd_buf.get<uint8_t>();

    ptr[0] = PAGE_PROGRAM_COMMAND;
    ptr[1] = (address >> 16) & 0xFF; // msb
    ptr[2] = (address >> 8) & 0xFF;
    ptr[3] = address & 0xFF; // lsb

    memcpy(ptr + 4, data, data_size);

    RESULT_UNWRAP(block_for_busy_bit());
    RESULT_UNWRAP(enable_write());

    // pin enable scope
    {
        auto pin = enable_chip();

        // first transmit command
        auto status = interface.transmit(ptr, total_size);
        RESULT_UNWRAP_OR(status, error::SPI);
    }

    // just in case
    force_disable();

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

void w25q128jv::force_disable()
{
    cs_pin.write(true);
}

success<w25q128jv::error> w25q128jv::block_for_busy_bit(uint32_t attempts)
{
    while (attempts-- > 0) {
        auto reg = read_status_register_1();
        RESULT_UNWRAP(reg);
        
        auto value = reg.unwrap();
        if ((value & 0x01) == 0) { 
            return success<error>();
        }
        osDelay(20);
    }
    return error::BUSY;
}

result<uint8_t, w25q128jv::error> w25q128jv::read_status_register_1()
{
    uint8_t tx_cmd[5];
    tx_cmd[0] = READ_STATUS_REGISTER_1_COMMAND;
    tx_cmd[1] = 0;
    tx_cmd[2] = 0;
    tx_cmd[3] = 0;
    tx_cmd[4] = 0;

    uint8_t rx_cmd[sizeof(tx_cmd)];
    rx_cmd[0] = 0;
    rx_cmd[1] = 0;
    rx_cmd[2] = 0;
    rx_cmd[3] = 0;
    rx_cmd[4] = 0;

    {
        auto pin = enable_chip();
        auto status = interface.transmit_receive(tx_cmd, rx_cmd, sizeof(tx_cmd));
        RESULT_UNWRAP_OR(status, error::SPI);
    }

    return rx_cmd[4];
}

} // namespace sdk
