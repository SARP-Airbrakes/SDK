#include <cstdint>

namespace sdk {
/**
 * Represents a generic CLI interface
 */
//transport class -> cli class

class Transport {
public: 
    virtual ~Transport() = default;

    virtual void print(const char* buf) = 0;
    virtual void printf(const char* format, ...)  = 0;

    virtual void receive(uint8_t* buf, uint32_t* len) = 0;
};


class cli {

private:

}