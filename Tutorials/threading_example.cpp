/**
 * @file threading_example.cpp
 * @brief Examples demonstrating threading primitives
 */

#include "hal_wrappers/Threading.hpp"
#include "hal_wrappers/Serial.hpp"
#include "hal_wrappers/DigitalOut.hpp"
#include "hal_wrappers/Timer.hpp"

using namespace sdk;

// Example 1: Circular Buffer for UART
CircularBuffer<char, 256> uart_rx_buffer;
Serial uart(A_9, A_10, 115200);

void uart_rx_handler() {
    char c;
    if (uart.getc(&c)) {
        uart_rx_buffer.push(c);
    }
}

void circular_buffer_example() {
    uart.write("Circular Buffer Example\r\n");
    uart.write("Type something...\r\n");
    
    // In real app, attach to UART interrupt
    while (1) {
        if (uart.readable()) {
            char c;
            uart.getc(&c);
            uart_rx_buffer.push(c);
        }
        
        // Process received data
        char data;
        while (uart_rx_buffer.pop(data)) {
            uart.putc(data);  // Echo
        }
        
        HAL_Delay(10);
    }
}

// Example 2: Mutex Protected Counter
Mutex counter_mutex;
volatile uint32_t shared_counter = 0;

void increment_counter() {
    LockGuard lock(counter_mutex);
    shared_counter++;
}

void mutex_example() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Mutex Example\r\n");
    
    Ticker ticker1, ticker2;
    
    ticker1.attach(increment_counter, 0.001f);
    ticker2.attach(increment_counter, 0.001f);
    
    while (1) {
        {
            LockGuard lock(counter_mutex);
            char msg[50];
            sprintf(msg, "Counter: %lu\r\n", shared_counter);
            uart.write(msg);
        }
        HAL_Delay(1000);
    }
}

// Example 3: Event Flags for Task Synchronization
EventFlags task_events;

#define EVENT_UART_RX   0x01
#define EVENT_TIMER     0x02
#define EVENT_SENSOR    0x04

void event_flags_example() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Event Flags Example\r\n");
    
    Ticker timer_tick;
    timer_tick.attach([]() {
        task_events.set(EVENT_TIMER);
    }, 1.0f);
    
    while (1) {
        // Wait for any event
        if (task_events.wait_any(EVENT_UART_RX | EVENT_TIMER | EVENT_SENSOR, 100)) {
            uint32_t flags = task_events.get();
            
            if (flags & EVENT_TIMER) {
                uart.write("Timer event!\r\n");
                task_events.clear(EVENT_TIMER);
            }
            
            if (flags & EVENT_UART_RX) {
                uart.write("UART event!\r\n");
                task_events.clear(EVENT_UART_RX);
            }
            
            if (flags & EVENT_SENSOR) {
                uart.write("Sensor event!\r\n");
                task_events.clear(EVENT_SENSOR);
            }
        }
    }
}

// Example 4: Message Queue
struct SensorData {
    uint32_t timestamp;
    float temperature;
    float humidity;
};

Queue<SensorData, 16> sensor_queue;

void producer_task() {
    static float temp = 20.0f;
    static float hum = 50.0f;
    
    SensorData data;
    data.timestamp = HAL_GetTick();
    data.temperature = temp;
    data.humidity = hum;
    
    sensor_queue.push(data);
    
    // Simulate sensor reading changes
    temp += 0.1f;
    hum += 0.2f;
    if (temp > 30.0f) temp = 20.0f;
    if (hum > 80.0f) hum = 50.0f;
}

void queue_example() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Message Queue Example\r\n");
    
    Ticker producer;
    producer.attach(producer_task, 0.5f);
    
    while (1) {
        SensorData data;
        if (sensor_queue.pop(data, 1000)) {
            char msg[100];
            sprintf(msg, "[%lu] Temp: %.1f°C, Humidity: %.1f%%\r\n",
                    data.timestamp, data.temperature, data.humidity);
            uart.write(msg);
        }
    }
}

// Example 5: Critical Section
volatile uint32_t interrupt_counter = 0;

void critical_section_example() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Critical Section Example\r\n");
    
    Ticker ticker;
    ticker.attach([]() {
        interrupt_counter++;
    }, 0.001f);
    
    while (1) {
        uint32_t count;
        {
            CriticalSection cs;
            count = interrupt_counter;
            interrupt_counter = 0;
        }
        
        char msg[50];
        sprintf(msg, "Interrupts per second: %lu\r\n", count);
        uart.write(msg);
        
        HAL_Delay(1000);
    }
}

// Example 6: Producer-Consumer Pattern
CircularBuffer<uint8_t, 64> work_queue;
volatile uint32_t produced = 0;
volatile uint32_t consumed = 0;

void producer() {
    static uint8_t value = 0;
    if (work_queue.push(value++)) {
        produced++;
    }
}

void producer_consumer_example() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Producer-Consumer Example\r\n");
    
    Ticker producer_ticker;
    producer_ticker.attach(producer, 0.01f);  // Produce every 10ms
    
    while (1) {
        // Consumer
        uint8_t data;
        while (work_queue.pop(data)) {
            consumed++;
            // Process data
        }
        
        char msg[100];
        sprintf(msg, "Produced: %lu, Consumed: %lu, Queue: %d/%d\r\n",
                produced, consumed, 
                work_queue.size(), work_queue.capacity());
        uart.write(msg);
        
        HAL_Delay(500);
    }
}

// Example 7: Multi-Task Simulation
Mutex serial_mutex;

void task_led() {
    static DigitalOut led(C_13);
    led = !led;
    
    LockGuard lock(serial_mutex);
    Serial uart(A_9, A_10, 115200);
    uart.write("Task LED\r\n");
}

void task_sensor() {
    LockGuard lock(serial_mutex);
    Serial uart(A_9, A_10, 115200);
    uart.write("Task Sensor\r\n");
}

void task_communication() {
    LockGuard lock(serial_mutex);
    Serial uart(A_9, A_10, 115200);
    uart.write("Task Communication\r\n");
}

void multitask_example() {
    Serial uart(A_9, A_10, 115200);
    uart.write("Multi-Task Example\r\n");
    
    Ticker led_ticker, sensor_ticker, comm_ticker;
    
    led_ticker.attach(task_led, 0.5f);
    sensor_ticker.attach(task_sensor, 1.0f);
    comm_ticker.attach(task_communication, 2.0f);
    
    while (1) {
        {
            LockGuard lock(serial_mutex);
            uart.write("Main loop\r\n");
        }
        HAL_Delay(3000);
    }
}

// Example 8: Buffered UART with Interrupts
class BufferedSerial {
public:
    BufferedSerial(Pin tx, Pin rx, uint32_t baud) : _serial(tx, rx, baud) {}
    
    void write(const char* str) {
        LockGuard lock(_tx_mutex);
        _serial.write(str);
    }
    
    bool read_line(char* buffer, size_t max_len, uint32_t timeout_ms = 1000) {
        uint32_t start = HAL_GetTick();
        size_t idx = 0;
        
        while ((HAL_GetTick() - start) < timeout_ms) {
            char c;
            if (_rx_buffer.pop(c)) {
                if (c == '\n' || c == '\r') {
                    buffer[idx] = '\0';
                    return true;
                }
                if (idx < max_len - 1) {
                    buffer[idx++] = c;
                }
            }
        }
        return false;
    }
    
    void rx_irq_handler() {
        char c;
        if (_serial.readable()) {
            _serial.getc(&c);
            _rx_buffer.push(c);
        }
    }

private:
    Serial _serial;
    CircularBuffer<char, 256> _rx_buffer;
    Mutex _tx_mutex;
};

void buffered_serial_example() {
    BufferedSerial uart(A_9, A_10, 115200);
    
    uart.write("Buffered Serial Example\r\n");
    uart.write("Send a line of text:\r\n");
    
    // Note: In real implementation, attach rx_irq_handler to UART interrupt
    
    while (1) {
        char line[128];
        if (uart.read_line(line, sizeof(line), 5000)) {
            uart.write("Received: ");
            uart.write(line);
            uart.write("\r\n");
        }
    }
}

int main() {
    HAL_Init();
    SystemClock_Config();
    
    // Run examples (uncomment one at a time)
    circular_buffer_example();
    // mutex_example();
    // event_flags_example();
    // queue_example();
    // critical_section_example();
    // producer_consumer_example();
    // multitask_example();
    // buffered_serial_example();
    
    while (1) {
        HAL_Delay(1000);
    }
}
