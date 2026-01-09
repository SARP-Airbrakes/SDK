/**
 * @file pwm_example.cpp
 * @brief Example demonstrating PWM wrapper usage
 * 
 * This example shows how to:
 * - Initialize PWM
 * - Control LED brightness
 * - Generate servo control signals
 * - Create various waveforms
 */

#include "hal_wrappers/PWM.hpp"
#include "hal_wrappers/Serial.hpp"

using namespace sdk;

// Example 1: LED Breathing Effect
void led_breathing_example() {
    PWM led(A_8, 1000); // 1kHz PWM on PA8
    
    while (1) {
        // Fade in
        for (float duty = 0.0f; duty <= 1.0f; duty += 0.01f) {
            led = duty;
            HAL_Delay(10);
        }
        
        // Fade out
        for (float duty = 1.0f; duty >= 0.0f; duty -= 0.01f) {
            led = duty;
            HAL_Delay(10);
        }
    }
}

// Example 2: Multiple PWM Outputs (RGB LED)
void rgb_led_example() {
    PWM red(A_8, 1000);   // TIM1_CH1
    PWM green(A_9, 1000); // TIM1_CH2
    PWM blue(A_10, 1000); // TIM1_CH3
    
    Serial uart(A_2, A_3, 115200);
    uart.write("RGB LED Example\r\n");
    
    while (1) {
        // Red
        uart.write("Red\r\n");
        red = 1.0f; green = 0.0f; blue = 0.0f;
        HAL_Delay(1000);
        
        // Green
        uart.write("Green\r\n");
        red = 0.0f; green = 1.0f; blue = 0.0f;
        HAL_Delay(1000);
        
        // Blue
        uart.write("Blue\r\n");
        red = 0.0f; green = 0.0f; blue = 1.0f;
        HAL_Delay(1000);
        
        // Yellow
        uart.write("Yellow\r\n");
        red = 1.0f; green = 1.0f; blue = 0.0f;
        HAL_Delay(1000);
        
        // Cyan
        uart.write("Cyan\r\n");
        red = 0.0f; green = 1.0f; blue = 1.0f;
        HAL_Delay(1000);
        
        // Magenta
        uart.write("Magenta\r\n");
        red = 1.0f; green = 0.0f; blue = 1.0f;
        HAL_Delay(1000);
        
        // White
        uart.write("White\r\n");
        red = 1.0f; green = 1.0f; blue = 1.0f;
        HAL_Delay(1000);
        
        // Rainbow fade
        uart.write("Rainbow\r\n");
        for (int i = 0; i < 360; i++) {
            float h = i / 360.0f;
            float r, g, b;
            
            // Simple HSV to RGB conversion (S=1, V=1)
            int hi = (int)(h * 6);
            float f = h * 6 - hi;
            
            switch (hi) {
                case 0: r = 1; g = f; b = 0; break;
                case 1: r = 1 - f; g = 1; b = 0; break;
                case 2: r = 0; g = 1; b = f; break;
                case 3: r = 0; g = 1 - f; b = 1; break;
                case 4: r = f; g = 0; b = 1; break;
                case 5: r = 1; g = 0; b = 1 - f; break;
                default: r = 0; g = 0; b = 0; break;
            }
            
            red = r; green = g; blue = b;
            HAL_Delay(10);
        }
    }
}

// Example 3: Servo Control (50Hz, 1-2ms pulse)
void servo_example() {
    PWM servo(B_6, 50); // 50Hz for servo control
    Serial uart(A_9, A_10, 115200);
    
    uart.write("Servo Control Example\r\n");
    uart.write("Sweeping 0-180 degrees\r\n");
    
    while (1) {
        // 0 degrees (1ms pulse = 5% duty cycle at 50Hz)
        uart.write("0 degrees\r\n");
        servo.pulse_width_ms(1);
        HAL_Delay(1000);
        
        // 90 degrees (1.5ms pulse = 7.5% duty cycle)
        uart.write("90 degrees\r\n");
        servo.pulse_width_ms(1.5f);
        HAL_Delay(1000);
        
        // 180 degrees (2ms pulse = 10% duty cycle)
        uart.write("180 degrees\r\n");
        servo.pulse_width_ms(2);
        HAL_Delay(1000);
        
        // Smooth sweep
        uart.write("Sweeping...\r\n");
        for (float pulse = 1.0f; pulse <= 2.0f; pulse += 0.01f) {
            servo.pulse_width_ms(pulse);
            HAL_Delay(20);
        }
        for (float pulse = 2.0f; pulse >= 1.0f; pulse -= 0.01f) {
            servo.pulse_width_ms(pulse);
            HAL_Delay(20);
        }
    }
}

// Example 4: Motor Speed Control
void motor_example() {
    PWM motor(B_7, 20000); // 20kHz PWM for motor control
    Serial uart(A_9, A_10, 115200);
    
    uart.write("Motor Speed Control\r\n");
    
    while (1) {
        // Accelerate
        uart.write("Accelerating...\r\n");
        for (float speed = 0.0f; speed <= 1.0f; speed += 0.01f) {
            motor = speed;
            HAL_Delay(50);
        }
        
        HAL_Delay(2000);
        
        // Decelerate
        uart.write("Decelerating...\r\n");
        for (float speed = 1.0f; speed >= 0.0f; speed -= 0.01f) {
            motor = speed;
            HAL_Delay(50);
        }
        
        HAL_Delay(2000);
    }
}

// Example 5: Frequency Sweep (Buzzer/Speaker)
void buzzer_example() {
    PWM buzzer(B_8, 1000, 0.5f); // Start at 1kHz, 50% duty cycle
    Serial uart(A_9, A_10, 115200);
    
    uart.write("Buzzer Frequency Sweep\r\n");
    
    while (1) {
        // Play musical notes (frequencies in Hz)
        const struct {
            const char* note;
            uint32_t freq;
        } notes[] = {
            {"C4", 262},
            {"D4", 294},
            {"E4", 330},
            {"F4", 349},
            {"G4", 392},
            {"A4", 440},
            {"B4", 494},
            {"C5", 523}
        };
        
        for (int i = 0; i < 8; i++) {
            uart.write(notes[i].note);
            uart.write("\r\n");
            buzzer.frequency(notes[i].freq);
            buzzer = 0.5f; // 50% duty cycle for square wave
            HAL_Delay(500);
        }
        
        buzzer = 0.0f; // Silence
        HAL_Delay(1000);
        
        // Frequency sweep
        uart.write("Frequency Sweep\r\n");
        for (uint32_t freq = 200; freq <= 2000; freq += 50) {
            buzzer.frequency(freq);
            buzzer = 0.5f;
            HAL_Delay(50);
        }
        
        buzzer = 0.0f;
        HAL_Delay(2000);
    }
}

// Example 6: PWM Reading (duty cycle monitoring)
void pwm_read_example() {
    PWM output(B_6, 1000);
    Serial uart(A_9, A_10, 115200);
    
    uart.write("PWM Duty Cycle Monitor\r\n");
    
    float duty = 0.0f;
    bool increasing = true;
    
    while (1) {
        output = duty;
        
        float read_duty = output.read();
        
        char msg[50];
        sprintf(msg, "Set: %.2f, Read: %.2f\r\n", duty, read_duty);
        uart.write(msg);
        
        if (increasing) {
            duty += 0.1f;
            if (duty >= 1.0f) {
                duty = 1.0f;
                increasing = false;
            }
        } else {
            duty -= 0.1f;
            if (duty <= 0.0f) {
                duty = 0.0f;
                increasing = true;
            }
        }
        
        HAL_Delay(500);
    }
}

int main() {
    HAL_Init();
    SystemClock_Config();
    
    // Run examples (uncomment one at a time)
    led_breathing_example();
    // rgb_led_example();
    // servo_example();
    // motor_example();
    // buzzer_example();
    // pwm_read_example();
    
    while (1) {
        HAL_Delay(1000);
    }
}
