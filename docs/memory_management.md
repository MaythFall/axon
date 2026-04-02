# Memory Management

This document provides the API reference and implementation details for **Axon's** core memory management utilities, including aligned allocation, memory pinning, and secure data destruction.

---

## Aligned Allocation

The `AlignedAllocator` is a C++20-compliant allocator designed for high-performance systems. It ensures data is aligned to cache-line or SIMD boundaries and handles security features like page-locking automatically.

### Template Parameters

* **`typename T`**: The type of elements to allocate.
* **`size_t Align`**: The alignment boundary in bytes (default: **64** for cache-line alignment).
* **`bool Lock`**: If **true**, the allocator pins memory to RAM on allocation (default: **true**).

### API Reference

* **`T* allocate(size_t n)`**: Allocates `n` elements. If `Lock` is true, it calls the OS API to prevent the memory from being swapped to disk.
* **`void deallocate(T* p, size_t n)`**: Automatically calls `secureWipe` and `unlockMemory` before freeing the pointer back to the heap.

### Usage

```cpp
// 64-byte aligned, memory-locked (High Security)
std::vector<uint8_t, axon::AlignedAllocator<uint8_t>> secure_data;

// 32-byte aligned, no memory locking (High Performance)
axon::AlignedAllocator<float, 32, false> fast_allocator;
```
> [!IMPORTANT]
> To force the library to throw an exception if a memory lock fails (e.g., due to OS `ulimit` restrictions on Linux), define `AXON_THROW_ON_LOCK_FAIL` before including the header.

---

## Memory Pinning

Axon replaces external dependencies like [`libsodium`](https://libsodium.gitbook.io/doc) with native OS system calls to "pin" sensitive data to physical RAM. This prevents the operating system from moving sensitive information to a swap file on the disk.

### API Reference

* **`bool lockMemory(void* addr, size_t len) noexcept`**
    * **Windows**: Wraps `VirtualLock`.
    * **Linux/POSIX**: Wraps `mlock`.
    * **Returns**: `true` if the memory was successfully pinned.
* **`void unlockMemory(void* addr, size_t len) noexcept`**
    * **Windows**: Wraps `VirtualUnlock`.
    * **Linux/POSIX**: Wraps `munlock`.

---

## Secure Data Wiping

Compilers often optimize away standard `memset` calls if a buffer is not accessed again before destruction. Axon provides a "guaranteed" wipe that uses platform-specific intrinsics to ensure data is zeroed out on the hardware level.

### API Reference

* **`void secureWipe(void *data, size_t size) noexcept`**
    * **Windows**: Uses `SecureZeroMemory`.
    * **Linux/POSIX**: Uses `explicit_bzero` or C23 `memset_explicit`.
    * **Fallback**: Uses a `volatile` pointer loop to bypass compiler optimization.

---

## Implementation Matrix

| Feature | Windows (MSVC) | Linux / POSIX |
| --- | --- | --- |
| **Allocation** | `_aligned_malloc` | `std::aligned_alloc` |
| **Deallocation** | `_aligned_free` | `std::free` |
| **Locking** | `VirtualLock` | `mlock` |
| **Wiping** | `SecureZeroMemory` | `explicit_bzero` |

### Requirements
* **Include**: `<axon/memory.hpp>`
* **Namespace**: `axon::`
* **Language**: C++20 or higher
