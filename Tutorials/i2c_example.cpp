/**
 * @file i2c_example.cpp
 * @brief Example demonstrating I2C wrapper usage
 * 
 * This example shows how to:
 * - Initialize I2C
 * - Scan for devices
 * - Read/write registers
 * - Communicate with common I2C sensors
 */

#include "hal_wrappers/I2C.hpp"
#include "hal_wrappers/DigitalOut.hpp"
#include "hal_wrappers/Serial.hpp"

using namespace sdk;

// Example 1: I2C Scanner
void i2c_scanner_example() {
    Serial uart(A_9, A_10, 115200);
    I2C i2c(B_7, B_6, 100000); // SDA=PB7, SCL=PB6, 100kHz
    
    uart.write("I2C Scanner Starting...\r\n");
    
    uint8_t found = 0;
    for (uint8_t addr = 1; addr < 128; addr++) {
        if (i2c.is_device_ready(addr, 1, 10)) {
            char msg[50];
            sprintf(msg, "Device found at 0x%02X\r\n", addr);
            uart.write(msg);
            found++;
        }
    }
    
    char msg[50];
    sprintf(msg, "Scan complete. Found %d devices.\r\n", found);
    uart.write(msg);
}

// Example 2: Read/Write to EEPROM (24C02 style)
void eeprom_example() {
    I2C i2c(B_7, B_6, 100000);
    uint8_t eeprom_addr = 0x50; // Common EEPROM address
    
    // Write single byte to address 0x00
    uint8_t write_data[] = {0x00, 0xAB}; // {memory_address, data}
    i2c.write(eeprom_addr, write_data, 2);
    HAL_Delay(5); // EEPROM write delay
    
    // Read back from address 0x00
    uint8_t mem_addr = 0x00;
    uint8_t read_data = 0;
    i2c.write(eeprom_addr, &mem_addr, 1); // Set address pointer
    i2c.read(eeprom_addr, &read_data, 1);  // Read data
    
    // Or use write_read
    i2c.write_read(eeprom_addr, &mem_addr, 1, &read_data, 1);
}

// Example 3: MPU6050 Accelerometer/Gyroscope
void mpu6050_example() {
    Serial uart(A_9, A_10, 115200);
    I2C i2c(B_7, B_6, 400000); // 400kHz for faster communication
    uint8_t mpu_addr = 0x68; // MPU6050 default address
    
    // Check device
    if (!i2c.is_device_ready(mpu_addr)) {
        uart.write("MPU6050 not found!\r\n");
        return;
    }
    
    // Wake up MPU6050 (write 0 to PWR_MGMT_1 register)
    i2c.write_register(mpu_addr, 0x6B, 0x00);
    HAL_Delay(100);
    
    // Read WHO_AM_I register (should return 0x68)
    uint8_t who_am_i;
    i2c.read_register(mpu_addr, 0x75, &who_am_i);
    
    char msg[50];
    sprintf(msg, "MPU6050 WHO_AM_I: 0x%02X\r\n", who_am_i);
    uart.write(msg);
    
    // Read accelerometer data (6 bytes starting at 0x3B)
    uint8_t accel_data[6];
    i2c.read_registers(mpu_addr, 0x3B, accel_data, 6);
    
    int16_t accel_x = (accel_data[0] << 8) | accel_data[1];
    int16_t accel_y = (accel_data[2] << 8) | accel_data[3];
    int16_t accel_z = (accel_data[4] << 8) | accel_data[5];
    
    sprintf(msg, "Accel X:%d Y:%d Z:%d\r\n", accel_x, accel_y, accel_z);
    uart.write(msg);
}

// Example 4: PCF8574 I/O Expander
void pcf8574_example() {
    I2C i2c(B_7, B_6, 100000);
    uint8_t io_addr = 0x20; // PCF8574 address
    
    // Write pattern to outputs
    uint8_t pattern = 0xAA; // Alternating pattern
    i2c.write(io_addr, &pattern, 1);
    HAL_Delay(500);
    
    pattern = 0x55;
    i2c.write(io_addr, &pattern, 1);
    HAL_Delay(500);
    
    // Read input state
    uint8_t input_state;
    i2c.read(io_addr, &input_state, 1);
}

int main() {
    HAL_Init();
    SystemClock_Config();
    
    // Run examples
    i2c_scanner_example();
    HAL_Delay(1000);
    
    // Uncomment the example you want to test:
    // eeprom_example();
    // mpu6050_example();
    // pcf8574_example();
    
    while (1) {
        HAL_Delay(1000);
    }
}
