#include "transport.hpp"
#include <cstdint>
#include <cstddef>
#include <cstring>

extern "C" {
uint8_t CDC_Transmit_FS(uint8_t* Buf, uint16_t Len);
}

namespace sdk {
    template <size_t N>
    class ring_buffer {
        static_assert(N >= 2, "ring buffer too small");
        private:
        uint8_t buffer[N];
        volatile uint32_t head = 0;
        volatile uint32_t tail = 0;
        uint32_t increment (uint32_t index) const
        {
            return (index + 1) % N;
        }
        public:
        ring_buffer() = default;
        ~ring_buffer() = default;
        size_t available () const
        {
            return (N + head - tail) % N;
        }

        size_t push(const uint8_t* data, size_t len) {
            size_t bytes_written = 0;
            while (bytes_written < len) {
                uint8_t next = increment(head);
                if (next == tail) {
                    // buffer full
                    break;
                }
                buffer[head] = data[bytes_written++];
                head = next;
            }

            return bytes_written;
        }

        size_t pop(uint8_t* data, size_t len) {
            size_t bytes_read = 0;
            while (bytes_read < len && tail != head) {
                data[bytes_read++] = buffer[tail];
                tail = increment(tail);
            }

            return bytes_read;
        }

        size_t free_space() const
        {
            return N - 1 - available();
        }
    };

    template <size_t RXBufferSize = 512, size_t TXBufferSize = 512>
    class usb_transport : public transport {
    public:
        usb_transport() = default;
        ~usb_transport() override = default;

        size_t available () const override
        {
            return rx_buffer.available();
        }

        size_t read(uint8_t* buf, size_t len) override
        {
            return rx_buffer.pop(buf, len);
        }

        size_t write(const uint8_t* buf, size_t len) override
        {
            const size_t pushed = tx_buffer.push(buf, len);
            if (pushed < len) {
                tx_dropped += (len - pushed);
            }
            return pushed;
        }

        void flush() override
        {
            // USB CDC does not have a flush operation
        }
    private:
        ring_buffer<RXBufferSize> rx_buffer;
        ring_buffer<TXBufferSize> tx_buffer;

        voltatile bool tx_in_progress = false;

        voltatile uint32_t rx_dropped = 0;
        voltatile uint32_t tx_dropped = 0;
    };
}