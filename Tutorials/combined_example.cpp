/**
 * @file combined_example.cpp
 * @brief Example combining multiple wrapper functionalities
 * 
 * This example shows real-world applications combining:
 * - I2C sensors
 * - SPI displays
 * - UART logging
 * - PWM outputs
 * - Analog inputs
 */

#include "hal_wrappers/I2C.hpp"
#include "hal_wrappers/SPI.hpp"
#include "hal_wrappers/Serial.hpp"
#include "hal_wrappers/PWM.hpp"
#include "hal_wrappers/AnalogIn.hpp"
#include "hal_wrappers/DigitalOut.hpp"

using namespace sdk;

// Example 1: Data Logger
// Reads I2C temperature sensor, ADC voltage, logs to UART
void data_logger_example() {
    Serial uart(A_9, A_10, 115200);
    I2C i2c(B_7, B_6, 400000);
    AnalogIn voltage(A_0);
    DigitalOut led(C_13);
    
    uint8_t temp_sensor_addr = 0x48; // Example: LM75 temperature sensor
    
    uart.write("\r\n=== Data Logger Started ===\r\n");
    uart.write("Timestamp, Temperature, Voltage\r\n");
    
    while (1) {
        // Read temperature from I2C sensor (LM75 format)
        uint8_t temp_data[2];
        if (i2c.read_registers(temp_sensor_addr, 0x00, temp_data, 2)) {
            int16_t raw_temp = (temp_data[0] << 8) | temp_data[1];
            float temperature = (raw_temp >> 5) * 0.125f;
            
            // Read voltage
            float voltage_val = voltage.read_voltage();
            
            // Log to UART
            char log[100];
            sprintf(log, "%lu, %.2f, %.3f\r\n", 
                    HAL_GetTick(), temperature, voltage_val);
            uart.write(log);
        } else {
            uart.write("ERROR: Failed to read temperature\r\n");
        }
        
        led = !led;
        HAL_Delay(1000);
    }
}

// Example 2: Climate Control System
// Reads temperature, controls fan speed with PWM
void climate_control_example() {
    Serial uart(A_9, A_10, 115200);
    I2C i2c(B_7, B_6, 400000);
    PWM fan(B_6, 25000); // 25kHz for fan control
    AnalogIn temp_sensor(A_0); // Analog temperature sensor
    
    uart.write("Climate Control System\r\n");
    
    const float TARGET_TEMP = 25.0f;
    const float TEMP_HYSTERESIS = 2.0f;
    
    while (1) {
        // Read temperature (using TMP36)
        float voltage = temp_sensor.read_voltage();
        float temperature = (voltage - 0.5f) * 100.0f;
        
        // Calculate fan speed based on temperature
        float error = temperature - TARGET_TEMP;
        float fan_speed = 0.0f;
        
        if (error > TEMP_HYSTERESIS) {
            // Too hot - increase fan speed proportionally
            fan_speed = (error - TEMP_HYSTERESIS) / 10.0f;
            fan_speed = (fan_speed > 1.0f) ? 1.0f : fan_speed;
        }
        
        fan = fan_speed;
        
        char msg[100];
        sprintf(msg, "Temp: %.1f°C | Target: %.1f°C | Fan: %.0f%%\r\n",
                temperature, TARGET_TEMP, fan_speed * 100.0f);
        uart.write(msg);
        
        HAL_Delay(500);
    }
}

// Example 3: Robot Controller
// Joystick input (ADC) controls motor speeds (PWM)
void robot_controller_example() {
    Serial uart(A_9, A_10, 115200);
    AnalogIn joy_x(A_0);
    AnalogIn joy_y(A_1);
    PWM motor_left(B_6, 20000);
    PWM motor_right(B_7, 20000);
    
    uart.write("Robot Controller\r\n");
    uart.write("Use joystick to control\r\n\r\n");
    
    while (1) {
        // Read joystick (center = 0.5)
        float x = (joy_x.read() - 0.5f) * 2.0f; // -1.0 to +1.0
        float y = (joy_y.read() - 0.5f) * 2.0f;
        
        // Arcade drive: mix x and y for tank drive
        float left = y + x;
        float right = y - x;
        
        // Clamp to -1.0 to +1.0
        left = (left > 1.0f) ? 1.0f : (left < -1.0f) ? -1.0f : left;
        right = (right > 1.0f) ? 1.0f : (right < -1.0f) ? -1.0f : right;
        
        // Convert to 0.0 to 1.0 for PWM (assuming motor driver handles direction)
        motor_left = (left + 1.0f) / 2.0f;
        motor_right = (right + 1.0f) / 2.0f;
        
        char msg[100];
        sprintf(msg, "X: %+.2f | Y: %+.2f | L: %.0f%% | R: %.0f%%\r\n",
                x, y, motor_left.read() * 100.0f, motor_right.read() * 100.0f);
        uart.write(msg);
        
        HAL_Delay(50);
    }
}

// Example 4: Sensor Hub
// Multiple sensors on I2C bus, data logging over UART
void sensor_hub_example() {
    Serial uart(A_9, A_10, 115200);
    I2C i2c(B_7, B_6, 400000);
    
    uart.write("Sensor Hub\r\n");
    uart.write("Scanning for sensors...\r\n");
    
    // Scan for devices
    uint8_t devices[10];
    uint8_t device_count = 0;
    
    for (uint8_t addr = 1; addr < 128; addr++) {
        if (i2c.is_device_ready(addr, 1, 10)) {
            char msg[50];
            sprintf(msg, "Found device: 0x%02X\r\n", addr);
            uart.write(msg);
            devices[device_count++] = addr;
            if (device_count >= 10) break;
        }
    }
    
    uart.write("\r\nReading sensor data...\r\n\r\n");
    
    while (1) {
        for (uint8_t i = 0; i < device_count; i++) {
            uint8_t addr = devices[i];
            uint8_t data[6];
            
            if (i2c.read(addr, data, 6, 100)) {
                char msg[100];
                sprintf(msg, "0x%02X: %02X %02X %02X %02X %02X %02X\r\n",
                        addr, data[0], data[1], data[2], data[3], data[4], data[5]);
                uart.write(msg);
            }
        }
        uart.write("\r\n");
        HAL_Delay(1000);
    }
}

// Example 5: Smart Lighting System
// Light sensor (ADC) and motion detector controls RGB LED (PWM)
void smart_lighting_example() {
    Serial uart(A_9, A_10, 115200);
    AnalogIn light_sensor(A_0);
    PWM red(A_8, 1000);
    PWM green(A_9, 1000);
    PWM blue(A_10, 1000);
    
    uart.write("Smart Lighting System\r\n");
    
    while (1) {
        float ambient_light = light_sensor.read();
        
        // Auto-adjust brightness based on ambient light
        float brightness = 1.0f - ambient_light; // Dimmer when bright
        
        // Time-based color temperature simulation
        uint32_t time_ms = HAL_GetTick();
        float time_factor = ((time_ms / 5000) % 10) / 10.0f; // 0-1 over 50s
        
        // Warm (orange) to cool (blue) transition
        red = brightness * (1.0f - time_factor * 0.5f);
        green = brightness * 0.8f;
        blue = brightness * time_factor;
        
        char msg[100];
        sprintf(msg, "Ambient: %.0f%% | Brightness: %.0f%% | Temp: %.0f%%\r\n",
                ambient_light * 100.0f, brightness * 100.0f, time_factor * 100.0f);
        uart.write(msg);
        
        HAL_Delay(100);
    }
}

// Example 6: Complete Automation System
void automation_system_example() {
    Serial uart(A_9, A_10, 115200);
    I2C i2c(B_7, B_6, 400000);
    AnalogIn temp_analog(A_0);
    AnalogIn light_sensor(A_1);
    PWM heater(B_6, 1000);
    PWM fan(B_7, 25000);
    DigitalOut relay(B_8);
    DigitalOut status_led(C_13);
    
    uart.write("\r\n");
    uart.write("=============================\r\n");
    uart.write("  Automation System v1.0\r\n");
    uart.write("=============================\r\n\r\n");
    
    bool heater_on = false;
    bool fan_on = false;
    bool lights_on = false;
    
    while (1) {
        // Read sensors
        float temp_voltage = temp_analog.read_voltage();
        float temperature = (temp_voltage - 0.5f) * 100.0f;
        float light_level = light_sensor.read();
        
        // Temperature control
        if (temperature < 20.0f && !heater_on) {
            heater = 1.0f;
            heater_on = true;
            uart.write("Heater ON\r\n");
        } else if (temperature > 22.0f && heater_on) {
            heater = 0.0f;
            heater_on = false;
            uart.write("Heater OFF\r\n");
        }
        
        if (temperature > 28.0f && !fan_on) {
            fan = 1.0f;
            fan_on = true;
            uart.write("Fan ON\r\n");
        } else if (temperature < 26.0f && fan_on) {
            fan = 0.0f;
            fan_on = false;
            uart.write("Fan OFF\r\n");
        }
        
        // Light control
        if (light_level < 0.3f && !lights_on) {
            relay = 1;
            lights_on = true;
            uart.write("Lights ON\r\n");
        } else if (light_level > 0.5f && lights_on) {
            relay = 0;
            lights_on = false;
            uart.write("Lights OFF\r\n");
        }
        
        // Status update
        char msg[150];
        sprintf(msg, "Temp: %.1f°C | Light: %.0f%% | Heater: %s | Fan: %s | Lights: %s\r\n",
                temperature, light_level * 100.0f,
                heater_on ? "ON" : "OFF",
                fan_on ? "ON" : "OFF",
                lights_on ? "ON" : "OFF");
        uart.write(msg);
        
        status_led = !status_led;
        HAL_Delay(1000);
    }
}

int main() {
    HAL_Init();
    SystemClock_Config();
    
    // Run examples (uncomment one at a time)
    // data_logger_example();
    // climate_control_example();
    // robot_controller_example();
    // sensor_hub_example();
    // smart_lighting_example();
    automation_system_example();
    
    while (1) {
        HAL_Delay(1000);
    }
}
