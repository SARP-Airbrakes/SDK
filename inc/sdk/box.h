
#ifndef SDK_BOX_H_
#define SDK_BOX_H_

#include <cstdlib>

namespace sdk {

/**
 * Represents a move-only, unique pointer to some memory on the heap that
 * automatically frees the memory when leaving scope.
 */
class box {
public:
    explicit box(size_t size);

    // take ownership of pointer
    explicit box(void *ptr) : ptr(ptr)
    {}

    ~box();

    // move-only
    box(const box &) = delete;
    box(box &&) = default;
    box &operator=(const box &) = delete;
    box &operator=(box &&) = default;

    void *get()
    {
        return ptr;
    }

    template<typename T>
    T *get()
    {
        return (T *) ptr;
    }

    bool is_ok()
    {
        return ptr != nullptr;
    }

private:
    void *ptr;

};

} // namespace sdk

#endif // SDK_UNIQUE_PTR_H_
