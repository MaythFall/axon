# API Documentation Index
This index serves as the central hub for the **Axon** technical reference. Each module is designed to be as lightweight as possible while providing robust systems-level primitives.

---

## Core Modules

### 1. [Memory Management](memory_management.md)
Hardware-level memory control and security.
* AlignedAllocator: C++20 allocator for cache-line alignment and page-locking.
* lockMemory / unlockMemory: Native OS page pinning.
* secureWipe: Unoptimizable memory zeroing.

### 2. [Secure Data Structures](secure_structs.md)
RAII-safe containers for sensitive information.
* SecureBuffer: A move-only, pinned, and auto-wiping container.
* SecureString: An aligned, secure alternative to std::string.

### 3. [Concurrency](concurrency.md)
Lock-free primitives for high-throughput messaging.
* RingBufferSPSC: Single-Producer Single-Consumer circular queue.
* hybridWait: Optimized spin-then-yield waiting strategy.

### 4. [Data Types](data_types.md)
Cross-platform high-precision arithmetic.
* uint128_t: Portable 128-bit unsigned integers.

### 5. [Error Management](error_management.md)
Specialized exception types for systems-level failures.
* memlock_failure: Thrown when memory pinning fails.

---

## Standard Compliance
* **Standard**: C++20
* **Alignment**: Defaulting to **64 bytes** for cache-line optimization.
* **Header-Only**: No compilation required; simply include and use.

```cpp
#include <axon/axon.hpp>
```