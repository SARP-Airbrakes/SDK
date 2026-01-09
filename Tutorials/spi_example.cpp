/**
 * @file spi_example.cpp
 * @brief Example demonstrating SPI wrapper usage
 * 
 * This example shows how to:
 * - Initialize SPI with different modes
 * - Transfer data
 * - Communicate with common SPI devices
 */

#include "hal_wrappers/SPI.hpp"
#include "hal_wrappers/DigitalOut.hpp"
#include "hal_wrappers/Serial.hpp"

using namespace sdk;

// Example 1: SPI Loopback Test (connect MOSI to MISO)
void spi_loopback_test() {
    Serial uart(A_9, A_10, 115200);
    SPI spi(A_7, A_6, A_5, 1000000, SPI::MODE_0); // MOSI, MISO, SCLK, 1MHz
    
    uart.write("SPI Loopback Test\r\n");
    uart.write("Connect MOSI (PA7) to MISO (PA6)\r\n");
    
    uint8_t test_data[] = {0xAA, 0x55, 0x12, 0x34, 0x56, 0x78};
    uint8_t rx_data[6] = {0};
    
    spi.transfer(test_data, rx_data, 6);
    
    bool pass = true;
    for (int i = 0; i < 6; i++) {
        char msg[50];
        sprintf(msg, "TX: 0x%02X  RX: 0x%02X %s\r\n", 
                test_data[i], rx_data[i], 
                (test_data[i] == rx_data[i]) ? "OK" : "FAIL");
        uart.write(msg);
        if (test_data[i] != rx_data[i]) pass = false;
    }
    
    uart.write(pass ? "Test PASSED\r\n" : "Test FAILED\r\n");
}

// Example 2: W25Q SPI Flash Memory
void w25q_flash_example() {
    Serial uart(A_9, A_10, 115200);
    SPI spi(A_7, A_6, A_5, 1000000, SPI::MODE_0);
    DigitalOut cs(A_4, 1); // Chip select
    
    uart.write("W25Q Flash Example\r\n");
    
    // Read JEDEC ID (0x9F command)
    cs = 0;
    uint8_t cmd = 0x9F;
    uint8_t id[3] = {0};
    spi.write(&cmd, 1);
    spi.read(id, 3);
    cs = 1;
    
    char msg[100];
    sprintf(msg, "Flash ID: 0x%02X%02X%02X\r\n", id[0], id[1], id[2]);
    uart.write(msg);
    
    // Read Status Register (0x05 command)
    cs = 0;
    cmd = 0x05;
    uint8_t status;
    spi.write(&cmd, 1);
    status = spi.write(0x00); // Dummy byte to read
    cs = 1;
    
    sprintf(msg, "Status Register: 0x%02X\r\n", status);
    uart.write(msg);
}

// Example 3: NRF24L01 Wireless Module
void nrf24_example() {
    Serial uart(A_9, A_10, 115200);
    SPI spi(A_7, A_6, A_5, 4000000, SPI::MODE_0); // NRF24 supports up to 8MHz
    DigitalOut cs(A_4, 1);
    DigitalOut ce(A_3, 0);
    
    uart.write("NRF24L01 Example\r\n");
    
    // Read CONFIG register (0x00)
    uint8_t read_cmd = 0x00; // Read command
    uint8_t config_val = 0;
    
    cs = 0;
    spi.write(&read_cmd, 1);
    config_val = spi.write(0x00); // Dummy byte
    cs = 1;
    
    char msg[50];
    sprintf(msg, "CONFIG register: 0x%02X\r\n", config_val);
    uart.write(msg);
    
    // Write CONFIG register (0x20 | 0x00 = 0x20 for write)
    uint8_t write_cmd[] = {0x20, 0x0E}; // Write CONFIG, PWR_UP + EN_CRC
    
    cs = 0;
    spi.write(write_cmd, 2);
    cs = 1;
    
    uart.write("NRF24 configured\r\n");
}

// Example 4: MAX7219 LED Matrix Driver
void max7219_example() {
    SPI spi(A_7, A_6, A_5, 1000000, SPI::MODE_0);
    DigitalOut cs(A_4, 1);
    
    // Helper function to write to MAX7219
    auto max7219_write = [&](uint8_t reg, uint8_t data) {
        cs = 0;
        uint8_t cmd[] = {reg, data};
        spi.write(cmd, 2);
        cs = 1;
    };
    
    // Initialize MAX7219
    max7219_write(0x09, 0x00); // Decode mode: no decode
    max7219_write(0x0A, 0x08); // Intensity: 50%
    max7219_write(0x0B, 0x07); // Scan limit: 8 digits
    max7219_write(0x0C, 0x01); // Shutdown: normal operation
    max7219_write(0x0F, 0x00); // Display test: off
    
    // Clear display
    for (uint8_t i = 1; i <= 8; i++) {
        max7219_write(i, 0x00);
    }
    
    // Draw a pattern (heart shape example)
    max7219_write(1, 0b01100110);
    max7219_write(2, 0b11111111);
    max7219_write(3, 0b11111111);
    max7219_write(4, 0b11111111);
    max7219_write(5, 0b01111110);
    max7219_write(6, 0b00111100);
    max7219_write(7, 0b00011000);
    max7219_write(8, 0b00000000);
}

// Example 5: Changing SPI modes
void spi_mode_test() {
    Serial uart(A_9, A_10, 115200);
    SPI spi(A_7, A_6, A_5);
    
    uart.write("SPI Mode Test\r\n");
    
    // Test all 4 SPI modes
    for (int mode = 0; mode < 4; mode++) {
        spi.format(static_cast<SPI::Mode>(mode));
        
        uint8_t test_val = spi.write(0xAA);
        
        char msg[50];
        sprintf(msg, "Mode %d: TX=0xAA RX=0x%02X\r\n", mode, test_val);
        uart.write(msg);
        
        HAL_Delay(100);
    }
}

int main() {
    HAL_Init();
    SystemClock_Config();
    
    // Run examples
    spi_loopback_test();
    HAL_Delay(1000);
    
    // Uncomment the example you want to test:
    // w25q_flash_example();
    // nrf24_example();
    // max7219_example();
    // spi_mode_test();
    
    while (1) {
        HAL_Delay(1000);
    }
}
