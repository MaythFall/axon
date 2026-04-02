# Portable Data Types
Axon provides `integers.hpp` to offer consistent, high-precision numeric types across different compilers and architectures. While this module is designed for expansion, it currently serves as the primary implementation for 128-bit unsigned arithmetic.

---

## uint128_t
The `uint128_t` type provides a portable 128-bit unsigned integer. It is designed to be a "drop-in" replacement for standard integer types where 64 bits of precision are insufficient.

### Implementation Architecture
* **Intrinsic Optimization**: On systems where the compiler supports it (such as GCC and Clang on x86_64), Axon detects and utilizes `__int128` to provide hardware-accelerated 128-bit math.
* **Compatibility Layer**: For compilers like MSVC that do not offer a native 128-bit type, Axon provides a robust struct-based implementation with manual carry-propagation logic.

---

## API Reference

### Initialization

```
// Initialize with high and low 64-bit components
axon::uint128_t my_int(0xFFFFFFFFFFFFFFFF, 0x0000000000000001);
```

### Accessors
* `low()`: Returns the lower 64-bit unsigned integer.
* `high()`: Returns the upper 64-bit unsigned integer.

### Arithmetic Operations
* `operator+` / `operator+=`: Addition with overflow carry.
* `operator-` / `operator-=`: Subtraction with borrow logic.

### Requirements
* **Include**: `axon/integers.hpp`
* **Namespace**: `axon::`