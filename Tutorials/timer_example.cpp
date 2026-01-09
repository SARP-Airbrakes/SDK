/**
 * @file timer_example.cpp
 * @brief Examples demonstrating Timer, Ticker, and Timeout usage
 */

#include "hal_wrappers/Timer.hpp"
#include "hal_wrappers/DigitalOut.hpp"
#include "hal_wrappers/Serial.hpp"

using namespace sdk;

// Example 1: Basic Ticker - LED Blink
void ticker_blink_example() {
    Serial uart(A_9, A_10, 115200);
    DigitalOut led(C_13);
    
    uart.write("Ticker Blink Example\r\n");
    
    Ticker ticker;
    
    ticker.attach([]() {
        static DigitalOut led(C_13);
        led = !led;
    }, 0.5f);  // Every 500ms
    
    while (1) {
        uart.write("Running...\r\n");
        HAL_Delay(2000);
    }
}

// Example 2: Multiple Tickers
volatile uint32_t fast_count = 0;
volatile uint32_t slow_count = 0;

void fast_callback() {
    fast_count++;
}

void slow_callback() {
    slow_count++;
}

void multiple_tickers_example() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Multiple Tickers Example\r\n");
    
    Ticker fast_ticker;
    Ticker slow_ticker;
    
    fast_ticker.attach(fast_callback, 0.1f);   // 10Hz
    slow_ticker.attach(slow_callback, 1.0f);   // 1Hz
    
    while (1) {
        char msg[100];
        sprintf(msg, "Fast: %lu, Slow: %lu\r\n", fast_count, slow_count);
        uart.write(msg);
        HAL_Delay(2000);
    }
}

// Example 3: Timeout - One-Shot Timer
void timeout_example() {
    Serial uart(A_9, A_10, 115200);
    DigitalOut led(C_13);
    
    uart.write("Timeout Example\r\n");
    uart.write("LED will turn off after 3 seconds\r\n");
    
    led = 0;  // Turn on
    
    Timeout timeout;
    timeout.attach([]() {
        static DigitalOut led(C_13);
        led = 1;  // Turn off
    }, 3.0f);
    
    while (1) {
        HAL_Delay(1000);
    }
}

// Example 4: PWM using Timer
void timer_pwm_example() {
    Serial uart(A_9, A_10, 115200);
    DigitalOut led(C_13);
    
    uart.write("Software PWM using Timer\r\n");
    
    float duty_cycle = 0.5f;  // 50%
    bool led_state = false;
    
    Ticker ticker;
    ticker.attach_us([&]() {
        static uint32_t counter = 0;
        counter++;
        if (counter >= 100) counter = 0;
        
        led = (counter < (duty_cycle * 100));
    }, 100);  // 100us = 10kHz
    
    while (1) {
        // Slowly change duty cycle
        for (duty_cycle = 0.0f; duty_cycle <= 1.0f; duty_cycle += 0.01f) {
            HAL_Delay(20);
        }
        for (duty_cycle = 1.0f; duty_cycle >= 0.0f; duty_cycle -= 0.01f) {
            HAL_Delay(20);
        }
    }
}

// Example 5: Watchdog Timer Simulation
volatile uint32_t watchdog_counter = 0;
volatile bool system_ok = true;

void watchdog_callback() {
    watchdog_counter++;
    if (watchdog_counter > 10) {  // 10 seconds without reset
        system_ok = false;
    }
}

void watchdog_example() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Watchdog Timer Example\r\n");
    
    Ticker watchdog;
    watchdog.attach(watchdog_callback, 1.0f);  // Check every second
    
    while (1) {
        if (system_ok) {
            // Simulate work
            uart.write("System OK\r\n");
            HAL_Delay(500);
            
            // Reset watchdog
            watchdog_counter = 0;
        } else {
            uart.write("WATCHDOG TIMEOUT! System reset needed.\r\n");
            HAL_Delay(1000);
            // In real system: NVIC_SystemReset();
        }
    }
}

// Example 6: Microsecond Timing
void microsecond_timing_example() {
    Serial uart(A_9, A_10, 115200);
    DigitalOut pulse(B_0);
    
    uart.write("Microsecond Timing Example\r\n");
    
    Ticker ticker;
    
    // Generate precise 100us pulse every millisecond
    ticker.attach_us([]() {
        static DigitalOut pulse(B_0);
        pulse = 1;
        Wait::us(100);
        pulse = 0;
    }, 1000);  // Every 1ms
    
    while (1) {
        uart.write("Generating pulses...\r\n");
        HAL_Delay(2000);
    }
}

// Example 7: Event Scheduler
struct ScheduledEvent {
    void (*callback)(void);
    uint32_t interval_ms;
    uint32_t last_run;
};

ScheduledEvent events[4];
uint8_t event_count = 0;

void event1() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Event 1 (100ms)\r\n");
}

void event2() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Event 2 (500ms)\r\n");
}

void event3() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Event 3 (1000ms)\r\n");
}

void scheduler_callback() {
    uint32_t now = HAL_GetTick();
    for (uint8_t i = 0; i < event_count; i++) {
        if ((now - events[i].last_run) >= events[i].interval_ms) {
            events[i].last_run = now;
            events[i].callback();
        }
    }
}

void event_scheduler_example() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Event Scheduler Example\r\n");
    
    // Register events
    events[0] = {event1, 100, 0};
    events[1] = {event2, 500, 0};
    events[2] = {event3, 1000, 0};
    event_count = 3;
    
    Ticker scheduler;
    scheduler.attach_us(scheduler_callback, 10000);  // Check every 10ms
    
    while (1) {
        HAL_Delay(5000);
    }
}

// Example 8: Stopwatch
void stopwatch_example() {
    Serial uart(A_9, A_10, 115200);
    Timer stopwatch;
    
    uart.write("Stopwatch Example\r\n");
    uart.write("Press button to start/stop\r\n");
    
    InterruptIn button(A_0, InterruptIn::FALLING);
    bool running = false;
    
    button.fall([&]() {
        if (!running) {
            stopwatch.reset();
            stopwatch.start();
            running = true;
        } else {
            stopwatch.stop();
            running = false;
        }
    });
    
    while (1) {
        char msg[50];
        sprintf(msg, "Time: %.3f seconds %s\r\n", 
                stopwatch.read(), 
                running ? "(running)" : "(stopped)");
        uart.write(msg);
        HAL_Delay(100);
    }
}

int main() {
    HAL_Init();
    SystemClock_Config();
    
    // Enable DWT for microsecond timing
    CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
    DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
    
    // Run examples (uncomment one at a time)
    ticker_blink_example();
    // multiple_tickers_example();
    // timeout_example();
    // timer_pwm_example();
    // watchdog_example();
    // microsecond_timing_example();
    // event_scheduler_example();
    // stopwatch_example();
    
    while (1) {
        HAL_Delay(1000);
    }
}
