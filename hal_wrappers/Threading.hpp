#pragma once
#include "base.hpp"

namespace sdk {

// ===== Thread-Safe Circular Buffer =====
template<typename T, size_t SIZE>
class CircularBuffer {
public:
    CircularBuffer() : _head(0), _tail(0), _full(false) {}
    
    bool push(const T& item) {
        __disable_irq();
        
        if (_full) {
            __enable_irq();
            return false;
        }
        
        _buffer[_head] = item;
        _head = (_head + 1) % SIZE;
        _full = (_head == _tail);
        
        __enable_irq();
        return true;
    }
    
    bool pop(T& item) {
        __disable_irq();
        
        if (empty()) {
            __enable_irq();
            return false;
        }
        
        item = _buffer[_tail];
        _tail = (_tail + 1) % SIZE;
        _full = false;
        
        __enable_irq();
        return true;
    }
    
    bool peek(T& item) const {
        __disable_irq();
        
        if (empty()) {
            __enable_irq();
            return false;
        }
        
        item = _buffer[_tail];
        __enable_irq();
        return true;
    }
    
    bool empty() const {
        return (!_full && (_head == _tail));
    }
    
    bool full() const {
        return _full;
    }
    
    size_t size() const {
        __disable_irq();
        
        size_t sz = SIZE;
        if (!_full) {
            if (_head >= _tail) {
                sz = _head - _tail;
            } else {
                sz = SIZE + _head - _tail;
            }
        }
        
        __enable_irq();
        return sz;
    }
    
    size_t capacity() const {
        return SIZE;
    }
    
    void clear() {
        __disable_irq();
        _head = 0;
        _tail = 0;
        _full = false;
        __enable_irq();
    }

private:
    T _buffer[SIZE];
    volatile size_t _head;
    volatile size_t _tail;
    volatile bool _full;
};

// ===== Critical Section Guard =====
class CriticalSection {
public:
    CriticalSection() {
        _primask = __get_PRIMASK();
        __disable_irq();
    }
    
    ~CriticalSection() {
        if (!_primask) {
            __enable_irq();
        }
    }
    
    CriticalSection(const CriticalSection&) = delete;
    CriticalSection& operator=(const CriticalSection&) = delete;

private:
    uint32_t _primask;
};

// ===== Simple Mutex (for bare-metal) =====
class Mutex {
public:
    Mutex() : _locked(false) {}
    
    void lock() {
        while (true) {
            __disable_irq();
            if (!_locked) {
                _locked = true;
                __enable_irq();
                break;
            }
            __enable_irq();
        }
    }
    
    void unlock() {
        __disable_irq();
        _locked = false;
        __enable_irq();
    }
    
    bool try_lock() {
        __disable_irq();
        if (!_locked) {
            _locked = true;
            __enable_irq();
            return true;
        }
        __enable_irq();
        return false;
    }

private:
    volatile bool _locked;
};

// ===== Lock Guard =====
class LockGuard {
public:
    explicit LockGuard(Mutex& mutex) : _mutex(mutex) {
        _mutex.lock();
    }
    
    ~LockGuard() {
        _mutex.unlock();
    }
    
    LockGuard(const LockGuard&) = delete;
    LockGuard& operator=(const LockGuard&) = delete;

private:
    Mutex& _mutex;
};

// ===== Event Flags =====
class EventFlags {
public:
    EventFlags() : _flags(0) {}
    
    void set(uint32_t flags) {
        __disable_irq();
        _flags |= flags;
        __enable_irq();
    }
    
    void clear(uint32_t flags) {
        __disable_irq();
        _flags &= ~flags;
        __enable_irq();
    }
    
    uint32_t get() const {
        return _flags;
    }
    
    bool wait_any(uint32_t flags, uint32_t timeout_ms = 0xFFFFFFFF) {
        uint32_t start = HAL_GetTick();
        while (true) {
            __disable_irq();
            if (_flags & flags) {
                __enable_irq();
                return true;
            }
            __enable_irq();
            
            if ((HAL_GetTick() - start) >= timeout_ms) {
                return false;
            }
        }
    }
    
    bool wait_all(uint32_t flags, uint32_t timeout_ms = 0xFFFFFFFF) {
        uint32_t start = HAL_GetTick();
        while (true) {
            __disable_irq();
            if ((_flags & flags) == flags) {
                __enable_irq();
                return true;
            }
            __enable_irq();
            
            if ((HAL_GetTick() - start) >= timeout_ms) {
                return false;
            }
        }
    }

private:
    volatile uint32_t _flags;
};

// ===== Message Queue =====
template<typename T, size_t SIZE>
class Queue {
public:
    Queue() {}
    
    bool push(const T& item, uint32_t timeout_ms = 0) {
        uint32_t start = HAL_GetTick();
        while (!_buffer.push(item)) {
            if (timeout_ms > 0 && (HAL_GetTick() - start) >= timeout_ms) {
                return false;
            }
        }
        return true;
    }
    
    bool pop(T& item, uint32_t timeout_ms = 0) {
        uint32_t start = HAL_GetTick();
        while (!_buffer.pop(item)) {
            if (timeout_ms > 0 && (HAL_GetTick() - start) >= timeout_ms) {
                return false;
            }
        }
        return true;
    }
    
    bool empty() const {
        return _buffer.empty();
    }
    
    bool full() const {
        return _buffer.full();
    }
    
    size_t size() const {
        return _buffer.size();
    }

private:
    CircularBuffer<T, SIZE> _buffer;
};

} // namespace sdk
