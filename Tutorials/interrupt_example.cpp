/**
 * @file interrupt_example.cpp
 * @brief Examples demonstrating InterruptIn usage
 */

#include "hal_wrappers/InterruptIn.hpp"
#include "hal_wrappers/DigitalOut.hpp"
#include "hal_wrappers/Serial.hpp"

using namespace sdk;

// Example 1: Simple Button Interrupt
void button_led_example() {
    Serial uart(A_9, A_10, 115200);
    DigitalOut led(C_13);
    
    uart.write("Button LED Example\r\n");
    uart.write("Press button to toggle LED\r\n");
    
    InterruptIn button(A_0, InterruptIn::FALLING);
    
    button.fall([]() {
        static DigitalOut led(C_13);
        led = !led;
    });
    
    while (1) {
        // Main loop idle
        HAL_Delay(100);
    }
}

// Example 2: Rising and Falling Edge Detection
DigitalOut status_led(C_13);
Serial debug_uart(A_9, A_10, 115200);

void rising_handler() {
    status_led = 1;
    debug_uart.write("Rising edge detected!\r\n");
}

void falling_handler() {
    status_led = 0;
    debug_uart.write("Falling edge detected!\r\n");
}

void edge_detection_example() {
    debug_uart.write("Edge Detection Example\r\n");
    
    InterruptIn signal(B_0, InterruptIn::BOTH);
    signal.rise(rising_handler);
    signal.fall(falling_handler);
    
    while (1) {
        HAL_Delay(100);
    }
}

// Example 3: Debounced Button
volatile uint32_t last_press_time = 0;
const uint32_t DEBOUNCE_MS = 200;
volatile uint32_t button_count = 0;

void debounced_button_handler() {
    uint32_t now = HAL_GetTick();
    if ((now - last_press_time) > DEBOUNCE_MS) {
        last_press_time = now;
        button_count++;
        
        Serial uart(A_9, A_10, 115200);
        char msg[50];
        sprintf(msg, "Button pressed %lu times\r\n", button_count);
        uart.write(msg);
    }
}

void debounced_button_example() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Debounced Button Example\r\n");
    
    InterruptIn button(A_0, InterruptIn::FALLING);
    button.mode(GPIO_PULLUP);
    button.fall(debounced_button_handler);
    
    while (1) {
        HAL_Delay(1000);
    }
}

// Example 4: Multiple Interrupts
volatile uint32_t int1_count = 0;
volatile uint32_t int2_count = 0;

void interrupt1_handler() {
    int1_count++;
}

void interrupt2_handler() {
    int2_count++;
}

void multiple_interrupts_example() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Multiple Interrupts Example\r\n");
    
    InterruptIn int1(B_0, InterruptIn::FALLING);
    InterruptIn int2(B_1, InterruptIn::FALLING);
    
    int1.fall(interrupt1_handler);
    int2.fall(interrupt2_handler);
    
    while (1) {
        char msg[100];
        sprintf(msg, "INT1: %lu, INT2: %lu\r\n", int1_count, int2_count);
        uart.write(msg);
        HAL_Delay(1000);
    }
}

// Example 5: Rotary Encoder
volatile int32_t encoder_position = 0;

void encoder_a_handler() {
    static InterruptIn encoder_b(B_1);
    if (encoder_b.read()) {
        encoder_position++;
    } else {
        encoder_position--;
    }
}

void rotary_encoder_example() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Rotary Encoder Example\r\n");
    
    InterruptIn encoder_a(B_0, InterruptIn::RISING);
    InterruptIn encoder_b(B_1);
    
    encoder_a.rise(encoder_a_handler);
    
    while (1) {
        char msg[50];
        sprintf(msg, "Position: %ld\r\n", encoder_position);
        uart.write(msg);
        HAL_Delay(100);
    }
}

int main() {
    HAL_Init();
    SystemClock_Config();
    
    // Enable DWT for cycle counter (needed for precise timing)
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    
    // Run examples (uncomment one at a time)
    button_led_example();
    // edge_detection_example();
    // debounced_button_example();
    // multiple_interrupts_example();
    // rotary_encoder_example();
    
    while (1) {
        HAL_Delay(1000);
    }
}
