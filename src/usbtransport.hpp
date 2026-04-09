#pragma once
#include "stdint.h"
#include <sdk/transport.hpp>
#include <cstddef>
#include <cstring>

#include "stm32f4xx_hal.h"
#include "usbd_def.h"

extern "C" {
uint8_t CDC_Transmit_FS(uint8_t* Buf, std::uint16_t Len);
}

#ifndef SDK_TRANSPORT_LOCK
  #define SDK_TRANSPORT_LOCK()   do { __disable_irq(); } while (0)
  #define SDK_TRANSPORT_UNLOCK() do { __enable_irq();  } while (0)
#endif

#ifndef SDK_CDC_MAX_PACKET
  #define SDK_CDC_MAX_PACKET 64
#endif

namespace sdk {

template <size_t N>
class ring_buffer {
    static_assert(N >= 2, "ring buffer too small");

public:
    ring_buffer() = default;
    ~ring_buffer() = default;

    size_t available() const {
        const uint32_t h = head_;
        const uint32_t t = tail_;
        return (h >= t) ? (h - t) : (N - (t - h));
    }

    size_t free_space() const { return (N - 1) - available(); }

    size_t push(const uint8_t* data, size_t len) {
        size_t bytes_written = 0;
        while (bytes_written < len) {
            const uint32_t next = inc_(head_);
            if (next == tail_) break;
            buffer_[head_] = data[bytes_written++];
            head_ = next;
        }
        return bytes_written;
    }

    size_t pop(uint8_t* out, size_t len) {
        size_t bytes_read = 0;
        while (bytes_read < len && tail_ != head_) {
            out[bytes_read++] = buffer_[tail_];
            tail_ = inc_(tail_);
        }
        return bytes_read;
    }

private:
    uint32_t inc_(uint32_t index) const { return (index + 1u) % N; }

    uint8_t buffer_[N]{};
    volatile uint32_t head_ = 0;
    volatile uint32_t tail_ = 0;
};

template <size_t RXBufferSize = 512, size_t TXBufferSize = 512, size_t MaxPacket = SDK_CDC_MAX_PACKET>
class usb_transport : public transport {
public:
    usb_transport() = default;
    ~usb_transport() override = default;

    void on_rx_isr(const uint8_t* data, size_t len) {
        const size_t pushed = rx_buffer_.push(data, len);
        if (pushed < len) rx_dropped_ += static_cast<uint32_t>(len - pushed);
    }

    void on_tx_complete_isr() { tx_in_progress_ = false; }

    size_t available() const override { return rx_buffer_.available(); }

    size_t read(uint8_t* buf, size_t len) override {
        return rx_buffer_.pop(buf, len);
    }

    size_t write(const uint8_t* buf, size_t len) override {
        SDK_TRANSPORT_LOCK();
        const size_t pushed = tx_buffer_.push(buf, len);
        SDK_TRANSPORT_UNLOCK();

        if (pushed < len) tx_dropped_ += static_cast<uint32_t>(len - pushed);
        return pushed;
    }

    void process_tx() override {
        if (tx_in_progress_) return;

        if (retry_pending_) {
            try_send_retry_();
            return;
        }

        size_t to_send = 0;

        SDK_TRANSPORT_LOCK();
        const size_t avail = tx_buffer_.available();
        if (avail != 0) {
            const size_t want = (avail < MaxPacket) ? avail : MaxPacket;
            to_send = tx_buffer_.pop(tx_stage_, want);
        }
        SDK_TRANSPORT_UNLOCK();

        if (to_send == 0) return;

        tx_in_progress_ = true;
        const uint8_t rc = CDC_Transmit_FS(tx_stage_, static_cast<uint16_t>(to_send));
        if (rc != USBD_OK) {
            tx_in_progress_ = false;
            retry_pending_ = true;
            retry_len_ = to_send;
        }
    }

    size_t tx_free() const { return tx_buffer_.free_space(); }
    size_t tx_queued() const { return tx_buffer_.available() + (retry_pending_ ? retry_len_ : 0); }

    uint32_t rx_dropped() const { return rx_dropped_; }
    uint32_t tx_dropped() const { return tx_dropped_; }

private:
    void try_send_retry_() {
        tx_in_progress_ = true;
        const uint8_t rc = CDC_Transmit_FS(tx_stage_, static_cast<uint16_t>(retry_len_));
        if (rc == USBD_OK) {
            retry_pending_ = false;
            retry_len_ = 0;
        } else {
            tx_in_progress_ = false;
        }
    }

private:
    ring_buffer<RXBufferSize> rx_buffer_;
    ring_buffer<TXBufferSize> tx_buffer_;

    uint8_t tx_stage_[MaxPacket]{};

    volatile bool tx_in_progress_ = false;

    bool retry_pending_ = false;
    size_t retry_len_ = 0;

    volatile uint32_t rx_dropped_ = 0;
    volatile uint32_t tx_dropped_ = 0;
};

} // namespace sdk
