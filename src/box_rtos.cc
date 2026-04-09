
#include <sdk/box.h>

#include <FreeRTOS.h>

namespace sdk {

box::box(size_t size)
{
    ptr = pvPortMalloc(size);
}

box::~box()
{
    if (is_ok()) {
        vPortFree(ptr);
        ptr = nullptr;
    }
}

} // namespace sdk
