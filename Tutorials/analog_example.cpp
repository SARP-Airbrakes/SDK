/**
 * @file analog_example.cpp
 * @brief Example demonstrating AnalogIn wrapper usage
 * 
 * This example shows how to:
 * - Initialize ADC
 * - Read analog values
 * - Monitor sensors
 * - Create voltage dividers
 */

#include "hal_wrappers/AnalogIn.hpp"
#include "hal_wrappers/Serial.hpp"
#include "hal_wrappers/PWM.hpp"

using namespace sdk;

// Example 1: Simple Analog Reading
void simple_adc_example() {
    Serial uart(A_9, A_10, 115200);
    AnalogIn pot(A_0); // Potentiometer on PA0
    
    uart.write("Simple ADC Example\r\n");
    uart.write("====================\r\n\r\n");
    
    while (1) {
        float normalized = pot.read();        // 0.0 to 1.0
        uint16_t value_16 = pot.read_u16();   // 0 to 65535
        uint16_t raw_12bit = pot.read_raw();  // 0 to 4095
        float voltage = pot.read_voltage();   // 0.0 to 3.3V
        
        char msg[100];
        sprintf(msg, "Norm: %.3f | 16-bit: %5u | 12-bit: %4u | Voltage: %.2fV\r\n",
                normalized, value_16, raw_12bit, voltage);
        uart.write(msg);
        
        HAL_Delay(500);
    }
}

// Example 2: ADC Controlled LED Brightness
void adc_led_control() {
    Serial uart(A_9, A_10, 115200);
    AnalogIn pot(A_0);
    PWM led(B_6, 1000);
    
    uart.write("ADC LED Control\r\n");
    uart.write("Adjust potentiometer to control LED brightness\r\n\r\n");
    
    while (1) {
        float brightness = pot.read();
        led = brightness;
        
        char msg[50];
        sprintf(msg, "Brightness: %.1f%%\r\n", brightness * 100.0f);
        uart.write(msg);
        
        HAL_Delay(100);
    }
}

// Example 3: Temperature Sensor (LM35 or TMP36)
void temperature_sensor_example() {
    Serial uart(A_9, A_10, 115200);
    AnalogIn temp_sensor(A_1);
    
    uart.write("Temperature Sensor Example\r\n");
    uart.write("============================\r\n\r\n");
    
    while (1) {
        float voltage = temp_sensor.read_voltage(3.3f);
        
        // For LM35: 10mV per degree C
        float temp_lm35 = voltage * 100.0f;
        
        // For TMP36: 10mV per degree C, 500mV offset
        float temp_tmp36 = (voltage - 0.5f) * 100.0f;
        
        char msg[100];
        sprintf(msg, "Voltage: %.3fV | LM35: %.1f°C | TMP36: %.1f°C\r\n",
                voltage, temp_lm35, temp_tmp36);
        uart.write(msg);
        
        HAL_Delay(1000);
    }
}

// Example 4: Light Sensor (Photoresistor/LDR)
void light_sensor_example() {
    Serial uart(A_9, A_10, 115200);
    AnalogIn ldr(A_2);
    PWM led(B_6, 1000);
    
    uart.write("Light Sensor Example\r\n");
    uart.write("LED brightness inversely proportional to light level\r\n\r\n");
    
    while (1) {
        float light_level = ldr.read();
        float led_brightness = 1.0f - light_level; // Inverse relationship
        
        led = led_brightness;
        
        char msg[50];
        sprintf(msg, "Light: %.1f%% | LED: %.1f%%\r\n",
                light_level * 100.0f, led_brightness * 100.0f);
        uart.write(msg);
        
        HAL_Delay(100);
    }
}

// Example 5: Battery Voltage Monitor
void battery_monitor_example() {
    Serial uart(A_9, A_10, 115200);
    AnalogIn battery(A_3);
    
    uart.write("Battery Voltage Monitor\r\n");
    uart.write("========================\r\n");
    uart.write("Connect battery through voltage divider (R1=10k, R2=10k for 2x)\r\n\r\n");
    
    while (1) {
        float adc_voltage = battery.read_voltage(3.3f);
        float battery_voltage = adc_voltage * 2.0f; // For 1:1 voltage divider
        
        // Calculate percentage (for 3.7V LiPo: 3.0V empty, 4.2V full)
        float percentage = ((battery_voltage - 3.0f) / (4.2f - 3.0f)) * 100.0f;
        percentage = (percentage < 0.0f) ? 0.0f : (percentage > 100.0f) ? 100.0f : percentage;
        
        char msg[100];
        sprintf(msg, "ADC: %.2fV | Battery: %.2fV | Level: %.0f%%\r\n",
                adc_voltage, battery_voltage, percentage);
        uart.write(msg);
        
        // Battery status
        if (battery_voltage < 3.3f) {
            uart.write("WARNING: Battery Low!\r\n");
        } else if (battery_voltage > 4.3f) {
            uart.write("WARNING: Overvoltage!\r\n");
        }
        
        uart.write("\r\n");
        HAL_Delay(2000);
    }
}

// Example 6: Multiple Channel Reading
void multi_channel_example() {
    Serial uart(A_9, A_10, 115200);
    AnalogIn ch0(A_0);
    AnalogIn ch1(A_1);
    AnalogIn ch2(A_2);
    AnalogIn ch3(A_3);
    
    uart.write("Multi-Channel ADC Example\r\n");
    uart.write("===========================\r\n\r\n");
    
    while (1) {
        float v0 = ch0.read_voltage();
        float v1 = ch1.read_voltage();
        float v2 = ch2.read_voltage();
        float v3 = ch3.read_voltage();
        
        char msg[150];
        sprintf(msg, "CH0: %.2fV | CH1: %.2fV | CH2: %.2fV | CH3: %.2fV\r\n",
                v0, v1, v2, v3);
        uart.write(msg);
        
        HAL_Delay(500);
    }
}

// Example 7: Joystick Reading (2-axis)
void joystick_example() {
    Serial uart(A_9, A_10, 115200);
    AnalogIn x_axis(A_0);
    AnalogIn y_axis(A_1);
    
    uart.write("Joystick Example\r\n");
    uart.write("==================\r\n\r\n");
    
    while (1) {
        float x = x_axis.read();
        float y = y_axis.read();
        
        // Convert to -1.0 to +1.0 range (assuming 0.5 is center)
        float x_centered = (x - 0.5f) * 2.0f;
        float y_centered = (y - 0.5f) * 2.0f;
        
        // Determine direction
        const char* direction = "CENTER";
        if (y_centered > 0.5f) direction = "UP";
        else if (y_centered < -0.5f) direction = "DOWN";
        else if (x_centered > 0.5f) direction = "RIGHT";
        else if (x_centered < -0.5f) direction = "LEFT";
        
        char msg[100];
        sprintf(msg, "X: %+.2f | Y: %+.2f | Direction: %s\r\n",
                x_centered, y_centered, direction);
        uart.write(msg);
        
        HAL_Delay(100);
    }
}

// Example 8: Averaging for Noise Reduction
void adc_averaging_example() {
    Serial uart(A_9, A_10, 115200);
    AnalogIn sensor(A_0);
    
    uart.write("ADC Averaging Example\r\n");
    uart.write("=======================\r\n\r\n");
    
    while (1) {
        // Single reading
        float single = sensor.read_voltage();
        
        // Averaged reading (16 samples)
        float sum = 0.0f;
        for (int i = 0; i < 16; i++) {
            sum += sensor.read_voltage();
            HAL_Delay(1);
        }
        float averaged = sum / 16.0f;
        
        char msg[100];
        sprintf(msg, "Single: %.3fV | Averaged: %.3fV | Diff: %.3fV\r\n",
                single, averaged, single - averaged);
        uart.write(msg);
        
        HAL_Delay(500);
    }
}

int main() {
    HAL_Init();
    SystemClock_Config();
    
    // Run examples (uncomment one at a time)
    simple_adc_example();
    // adc_led_control();
    // temperature_sensor_example();
    // light_sensor_example();
    // battery_monitor_example();
    // multi_channel_example();
    // joystick_example();
    // adc_averaging_example();
    
    while (1) {
        HAL_Delay(1000);
    }
}
