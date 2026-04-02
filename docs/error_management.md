# Error Management
Axon uses a lean error-handling strategy, prioritizing standard C++ exceptions for fatal initialization failures while maintaining a minimal footprint for high-performance paths.

---

## Exception Hierarchy
The library provides specialized exception types within the `axon::` namespace. These inherit from `std::runtime_error` to ensure they are easily caught by standard error-handling logic.

### memlock_failure
This exception is thrown when the library is unable to pin memory to physical RAM. This typically occurs if the operating system's `ulimit` for locked memory is too low or if the system has exhausted its available physical memory.

---

## Usage Patterns
If you define `AXON_THROW_ON_LOCK_FAIL` in your project, Axon components will throw `memlock_failure` instead of failing silently.

```cpp
try {
    // Attempting to allocate pinned memory
    axon::SecureBuffer<uint8_t> key(64);
} catch (const axon::memlock_failure& e) {
    std::cerr << "Security Critical: Could not pin memory. " << e.what() << std::endl;
    // Handle graceful shutdown or fallback
}
```

### Requirements
* **Include**: `axon/errors.hpp`
* **Namespace**: `axon::`