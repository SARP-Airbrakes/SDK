# STM32F4 SDK

A modern, easy-to-use C++ SDK for STM32F4 microcontrollers with Arduino-like simplicity and full HAL power.

## Features

- 🚀 **Arduino-style API** - Simple, intuitive interfaces
- ⚡ **Zero CubeMX Required** - Just instantiate and go
- 🎯 **Type-safe** - Modern C++ with compile-time checking
- 🔌 **Complete HAL Wrappers** - All peripherals covered
- 📚 **Extensive Examples** - Real-world usage patterns
- 🧵 **Threading Support** - Mutexes, queues, event flags

## Supported Peripherals

### Digital I/O
- `DigitalIn` - Digital input with pull-up/down
- `DigitalOut` - Digital output
- `DigitalInOut` - Bidirectional digital pin
- `InterruptIn` - External interrupts with callbacks

### Analog I/O
- `AnalogIn` - 12-bit ADC input
- `PWM` - PWM output with frequency/duty cycle control

### Communication
- `Serial` - UART/USART communication
- `SoftwareSerial` - Bit-banged serial (any pins)
- `I2C` - I2C master mode
- `SPI` - SPI master mode

### Timing
- `Timer` - Hardware timers with callbacks
- `Ticker` - Periodic interrupt callbacks
- `Timeout` - One-shot timers
- `Wait` - Blocking delay functions

### Threading Primitives
- `Mutex` - Mutual exclusion locks
- `CriticalSection` - Interrupt-safe sections
- `CircularBuffer<T, SIZE>` - Thread-safe ring buffer
- `Queue<T, SIZE>` - Message queue
- `EventFlags` - Event synchronization

## Quick Start

### Installation

1. Clone this repository into your project
2. Include the SDK in your project:
   ```cpp
   #include "base.hpp"
   #include "hal_wrappers/DigitalOut.hpp"
   #include "hal_wrappers/Serial.hpp"
   // ... other includes
   ```

### Minimal Example

```cpp
#include "hal_wrappers/DigitalOut.hpp"
#include "hal_wrappers/Serial.hpp"

using namespace sdk;

int main() {
    HAL_Init();
    SystemClock_Config();
    
    DigitalOut led(C_13);
    Serial uart(A_9, A_10, 115200);
    
    uart.write("Hello, World!\r\n");
    
    while (1) {
        led = !led;
        HAL_Delay(1000);
    }
}
```

## API Reference

### Digital I/O

#### DigitalOut
```cpp
DigitalOut led(C_13);           // Create output on PC13
led = 1;                        // Set high
led = 0;                        // Set low
led.write(1);                   // Alternative syntax
int state = led.read();         // Read back state
```

#### DigitalIn
```cpp
DigitalIn button(A_0, GPIO_PULLUP);  // Input with pull-up
if (button) {                        // Read value
    // Button pressed
}
```

#### InterruptIn
```cpp
InterruptIn button(B_0, InterruptIn::FALLING);

void button_pressed() {
    // Handle interrupt
}

button.fall(button_pressed);    // Attach callback
button.rise(another_callback);  // Rising edge
```

### Analog I/O

#### AnalogIn
```cpp
AnalogIn pot(A_0);              // ADC on PA0
float value = pot.read();       // 0.0 to 1.0
uint16_t raw = pot.read_u16();  // 0 to 65535
float voltage = pot.read_voltage(3.3f);  // Volts
```

#### PWM
```cpp
PWM motor(B_6, 20000);          // 20kHz PWM
motor = 0.5f;                   // 50% duty cycle
motor.write(0.75f);             // 75% duty cycle
motor.frequency(1000);          // Change frequency
motor.pulse_width_ms(1.5f);     // For servo control
```

### Communication

#### Serial (UART)
```cpp
Serial uart(A_9, A_10, 115200); // TX, RX, baud
uart.write("Hello\r\n");
uart.putc('A');

char c;
if (uart.readable()) {
    uart.getc(&c);
}
```

#### I2C
```cpp
I2C i2c(B_7, B_6, 400000);      // SDA, SCL, 400kHz

// Device scanning
if (i2c.is_device_ready(0x68)) {
    // Device found
}

// Register access
uint8_t value;
i2c.read_register(0x68, 0x75, &value);
i2c.write_register(0x68, 0x6B, 0x00);

// Bulk read
uint8_t data[6];
i2c.read_registers(0x68, 0x3B, data, 6);
```

#### SPI
```cpp
SPI spi(A_7, A_6, A_5, 1000000, SPI::MODE_0);  // MOSI, MISO, SCLK
DigitalOut cs(A_4, 1);          // Chip select

cs = 0;
uint8_t rx = spi.write(0xAA);   // Send & receive byte
cs = 1;

// Bulk transfer
uint8_t tx_data[4] = {1, 2, 3, 4};
uint8_t rx_data[4];
spi.transfer(tx_data, rx_data, 4);
```

### Timing

#### Timer & Ticker
```cpp
Ticker ticker;

void periodic_callback() {
    // Called every 500ms
}

ticker.attach(periodic_callback, 0.5f);  // Every 0.5 seconds
ticker.attach_us(periodic_callback, 500000);  // Microseconds
ticker.detach();                              // Stop
```

#### Timeout
```cpp
Timeout timeout;

void delayed_function() {
    // Called once after delay
}

timeout.attach(delayed_function, 2.0f);  // After 2 seconds
```

#### Wait
```cpp
Wait::ms(1000);     // Wait 1 second
Wait::us(500);      // Wait 500 microseconds
Wait::s(0.5f);      // Wait 0.5 seconds
```

### Threading Primitives

#### Mutex
```cpp
Mutex mutex;

void protected_function() {
    LockGuard lock(mutex);  // Automatic lock/unlock
    // Critical section
}  // Automatically unlocks
```

#### CircularBuffer
```cpp
CircularBuffer<uint8_t, 128> buffer;

buffer.push(0xAA);
uint8_t data;
if (buffer.pop(data)) {
    // Got data
}
```

#### Event Flags
```cpp
EventFlags events;

#define EVENT_A 0x01
#define EVENT_B 0x02

events.set(EVENT_A);
events.wait_any(EVENT_A | EVENT_B, 1000);  // Wait up to 1 sec
```

## Pin Naming Convention

Pins are named using the format `PORT_NUMBER`:
- `A_0` to `A_15` - GPIOA pins
- `B_0` to `B_15` - GPIOB pins
- `C_0` to `C_15` - GPIOC pins
- `D_0` to `D_15` - GPIOD pins
- `E_0` to `E_15` - GPIOE pins

Example: `sdk::B_6` refers to pin PB6

## Peripheral Auto-Detection

The SDK automatically detects which peripheral instance to use based on pins:

### I2C
- `B_6/B_7` or `B_8/B_9` → I2C1
- `B_10/B_11` → I2C2
- `A_8/C_9` → I2C3

### SPI
- `A_5/A_6/A_7` → SPI1
- `B_13/B_14/B_15` → SPI2
- `C_10/C_11/C_12` → SPI3

### UART
- `A_9/A_10` or `B_6/B_7` → USART1
- `A_2/A_3` → USART2
- `C_6/C_7` → USART6

See `pinmap.hpp` for complete pin mapping tables.

## Examples

### LED Blink
```cpp
#include "hal_wrappers/DigitalOut.hpp"
using namespace sdk;

int main() {
    HAL_Init();
    SystemClock_Config();
    
    DigitalOut led(C_13);
    
    while (1) {
        led = !led;
        HAL_Delay(500);
    }
}
```

### UART Echo
```cpp
#include "hal_wrappers/Serial.hpp"
using namespace sdk;

int main() {
    HAL_Init();
    SystemClock_Config();
    
    Serial uart(A_9, A_10, 115200);
    uart.write("Ready\r\n");
    
    while (1) {
        if (uart.readable()) {
            char c;
            uart.getc(&c);
            uart.putc(c);  // Echo
        }
    }
}
```

### I2C Sensor Reading
```cpp
#include "hal_wrappers/I2C.hpp"
#include "hal_wrappers/Serial.hpp"
using namespace sdk;

int main() {
    HAL_Init();
    SystemClock_Config();
    
    Serial uart(A_9, A_10, 115200);
    I2C i2c(B_7, B_6, 400000);
    
    uint8_t sensor_addr = 0x68;
    
    while (1) {
        uint8_t data[6];
        if (i2c.read_registers(sensor_addr, 0x3B, data, 6)) {
            int16_t x = (data[0] << 8) | data[1];
            int16_t y = (data[2] << 8) | data[3];
            int16_t z = (data[4] << 8) | data[5];
            
            char msg[50];
            sprintf(msg, "X:%d Y:%d Z:%d\r\n", x, y, z);
            uart.write(msg);
        }
        HAL_Delay(100);
    }
}
```

### Interrupt-Driven LED
```cpp
#include "hal_wrappers/InterruptIn.hpp"
#include "hal_wrappers/DigitalOut.hpp"
using namespace sdk;

DigitalOut led(C_13);

void toggle_led() {
    led = !led;
}

int main() {
    HAL_Init();
    SystemClock_Config();
    
    InterruptIn button(A_0, InterruptIn::FALLING);
    button.fall(toggle_led);
    
    while (1) {
        // Main loop does nothing
    }
}
```

More examples in the `Tutorials/` directory.

## Project Structure

```
SDK/
├── base.hpp              # Core SDK definitions
├── pinmap.hpp            # Pin mappings and lookup tables
├── hal_wrappers/         # Peripheral wrappers
│   ├── DigitalIn.hpp
│   ├── DigitalOut.hpp
│   ├── DigitalInOut.hpp
│   ├── InterruptIn.hpp
│   ├── AnalogIn.hpp
│   ├── PWM.hpp
│   ├── Serial.hpp
│   ├── SoftwareSerial.hpp
│   ├── I2C.hpp
│   ├── SPI.hpp
│   ├── Timer.hpp
│   └── Threading.hpp
├── sensors/              # Sensor drivers
│   └── bno055.h
└── Tutorials/            # Example code
    ├── i2c_example.cpp
    ├── spi_example.cpp
    ├── serial_example.cpp
    ├── pwm_example.cpp
    ├── analog_example.cpp
    └── combined_example.cpp
```

## Requirements

- STM32F4 microcontroller
- STM32 HAL Library
- C++11 or later compiler
- Basic `SystemClock_Config()` function (from CubeMX or manual)

## Thread Safety

The following classes are interrupt-safe:
- `CircularBuffer` - Uses critical sections
- `Mutex` - Atomic operations
- `EventFlags` - Atomic flag operations
- `Queue` - Built on CircularBuffer

For bare-metal applications, use `CriticalSection` or `Mutex` to protect shared resources.

## Performance Notes

- **SoftwareSerial** - Limited to ~9600 baud reliably
- **Hardware Serial** - Up to 4.5 Mbps (depends on clock)
- **SPI** - Up to 42 MHz (APB2/2)
- **I2C** - Up to 400 kHz (Fast Mode)
- **PWM** - Resolution depends on frequency (higher freq = lower resolution)

## Contributing

Contributions welcome! Please:
1. Follow the existing code style
2. Add examples for new features
3. Update this README
4. Test on real hardware

## License

MIT License - See LICENSE file for details

## Support

For issues, questions, or suggestions:
- Open an issue on GitHub
- Check the examples in `Tutorials/`
- Review the inline documentation in header files

## Roadmap

- [ ] CAN bus support
- [ ] USB device/host
- [ ] DMA wrappers
- [ ] RTC implementation
- [ ] Flash memory read/write
- [ ] FreeRTOS integration
- [ ] More sensor drivers
- [ ] Unit tests

---

**Happy coding! 🚀**
