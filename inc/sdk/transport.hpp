#pragma once
#include <cstddef>
#include <cstdint>

namespace sdk {
    class transport {
    public:
        virtual ~transport() = default;

        virtual size_t available () const = 0;
        virtual size_t read(uint8_t* buf, size_t len) = 0;
        virtual size_t write(const uint8_t* buf, size_t len) = 0;
        virtual void process_tx() = 0;
    };
}