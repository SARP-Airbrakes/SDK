/**
 * @file serial_example.cpp
 * @brief Example demonstrating Serial (UART) wrapper usage
 * 
 * This example shows how to:
 * - Initialize UART
 * - Send and receive data
 * - Echo characters
 * - Parse commands
 */

#include "hal_wrappers/Serial.hpp"
#include "hal_wrappers/DigitalOut.hpp"

using namespace sdk;

// Example 1: Simple Echo
void echo_example() {
    Serial uart(A_9, A_10, 115200);
    
    uart.write("Serial Echo Test\r\n");
    uart.write("Type something...\r\n");
    
    while (1) {
        if (uart.readable()) {
            char c;
            uart.getc(&c);
            uart.putc(c); // Echo back
        }
    }
}

// Example 2: Formatted Output
void printf_example() {
    Serial uart(A_9, A_10, 115200);
    DigitalOut led(C_13);
    
    uart.write("Formatted Output Example\r\n");
    uart.write("=========================\r\n\r\n");
    
    uint32_t counter = 0;
    
    while (1) {
        char buffer[100];
        
        // System tick
        sprintf(buffer, "Counter: %lu, Tick: %lu\r\n", counter, HAL_GetTick());
        uart.write(buffer);
        
        // Hex values
        sprintf(buffer, "Hex: 0x%04X, Binary: ", counter & 0xFFFF);
        uart.write(buffer);
        
        // Print binary representation
        for (int i = 7; i >= 0; i--) {
            uart.putc((counter & (1 << i)) ? '1' : '0');
        }
        uart.write("\r\n\r\n");
        
        led = !led;
        counter++;
        HAL_Delay(1000);
    }
}

// Example 3: Command Parser
void command_parser_example() {
    Serial uart(A_9, A_10, 115200);
    DigitalOut led(C_13);
    
    uart.write("\r\n================================\r\n");
    uart.write("Command Parser Example\r\n");
    uart.write("================================\r\n");
    uart.write("Commands:\r\n");
    uart.write("  led on  - Turn LED on\r\n");
    uart.write("  led off - Turn LED off\r\n");
    uart.write("  status  - Show status\r\n");
    uart.write("  help    - Show this help\r\n");
    uart.write("================================\r\n\r\n");
    uart.write("> ");
    
    char cmd_buffer[64];
    uint8_t cmd_idx = 0;
    
    while (1) {
        if (uart.readable()) {
            char c;
            uart.getc(&c);
            
            if (c == '\r' || c == '\n') {
                if (cmd_idx > 0) {
                    cmd_buffer[cmd_idx] = '\0';
                    uart.write("\r\n");
                    
                    // Process command
                    if (strcmp(cmd_buffer, "led on") == 0) {
                        led = 0; // Active low on most boards
                        uart.write("LED turned ON\r\n");
                    }
                    else if (strcmp(cmd_buffer, "led off") == 0) {
                        led = 1;
                        uart.write("LED turned OFF\r\n");
                    }
                    else if (strcmp(cmd_buffer, "status") == 0) {
                        char msg[100];
                        sprintf(msg, "LED: %s, Uptime: %lu ms\r\n", 
                                led ? "OFF" : "ON", HAL_GetTick());
                        uart.write(msg);
                    }
                    else if (strcmp(cmd_buffer, "help") == 0) {
                        uart.write("Available commands: led on, led off, status, help\r\n");
                    }
                    else {
                        uart.write("Unknown command: ");
                        uart.write(cmd_buffer);
                        uart.write("\r\n");
                    }
                    
                    cmd_idx = 0;
                    uart.write("> ");
                }
            }
            else if (c == 127 || c == 8) { // Backspace
                if (cmd_idx > 0) {
                    cmd_idx--;
                    uart.write("\b \b"); // Erase character
                }
            }
            else if (cmd_idx < 63) {
                cmd_buffer[cmd_idx++] = c;
                uart.putc(c); // Echo
            }
        }
    }
}

// Example 4: Binary Data Transfer
void binary_transfer_example() {
    Serial uart(A_9, A_10, 115200);
    
    uart.write("Binary Data Transfer Example\r\n");
    
    // Send binary data
    uint8_t data[] = {0x01, 0x02, 0x03, 0x04, 0x05};
    uart.write(data, sizeof(data));
    
    // Receive binary data
    uint8_t rx_buffer[10];
    if (uart.read(rx_buffer, 10, 5000)) {
        uart.write("Received 10 bytes\r\n");
    } else {
        uart.write("Timeout waiting for data\r\n");
    }
}

// Example 5: Multiple UARTs
void multi_uart_example() {
    Serial uart1(A_9, A_10, 115200);  // USART1
    Serial uart2(A_2, A_3, 9600);     // USART2
    
    uart1.write("UART1: Ready\r\n");
    uart2.write("UART2: Ready\r\n");
    
    while (1) {
        // Forward data from UART2 to UART1
        if (uart2.readable()) {
            char c;
            uart2.getc(&c);
            uart1.putc(c);
        }
        
        // Forward data from UART1 to UART2
        if (uart1.readable()) {
            char c;
            uart1.getc(&c);
            uart2.putc(c);
        }
    }
}

// Example 6: GPS NMEA Parser (conceptual)
void gps_example() {
    Serial uart(A_9, A_10, 9600); // GPS typically use 9600 baud
    
    char nmea_buffer[128];
    uint8_t idx = 0;
    
    while (1) {
        if (uart.readable()) {
            char c;
            uart.getc(&c);
            
            if (c == '$') {
                idx = 0;
                nmea_buffer[idx++] = c;
            }
            else if (c == '\n') {
                nmea_buffer[idx] = '\0';
                
                // Check if it's a GPRMC sentence
                if (strncmp(nmea_buffer, "$GPRMC", 6) == 0) {
                    uart.write("Position: ");
                    uart.write(nmea_buffer);
                    uart.write("\r\n");
                }
                idx = 0;
            }
            else if (idx < 127) {
                nmea_buffer[idx++] = c;
            }
        }
    }
}

int main() {
    HAL_Init();
    SystemClock_Config();
    
    // Run examples (uncomment one at a time)
    // echo_example();
    printf_example();
    // command_parser_example();
    // binary_transfer_example();
    // multi_uart_example();
    // gps_example();
    
    while (1) {
        HAL_Delay(1000);
    }
}
