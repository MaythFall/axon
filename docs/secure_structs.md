# Secure Data Structures
This document covers the RAII wrappers provided by **Axon** to ensure sensitive data is handled securely throughout its lifecycle.

## SecureBuffer
`SecureBuffer<T, Align>` is a specialized container that combines the convenience of `std::vector` with the security of Axon's `AlignedAllocator`.

### Key Features
* **Automatic Page Pinning**: Memory is locked to RAM on allocation to prevent the OS from swapping sensitive data to a disk.
* **Guaranteed Wipe**: All memory is zeroed out using `secureWipe` before being released to the OS.
* **Move-Only Semantics**: Copies are deleted to prevent accidental duplication of sensitive data in memory.

### API Reference
* **`explicit SecureBuffer(size_t size)`**: Allocates and pins `size` elements of type `T`.
* **`void resize(size_t count)`**: Safely resizes the buffer.
* **`T* data()`**: Returns a raw pointer to the underlying aligned memory.
* **`emplace_back(Args&&... args)`**: Constructs an element in-place. Highly efficient for building secure sets of data.

## SecureString
`SecureString` is a type alias for `std::basic_string` using the `axon::AlignedAllocator`. It behaves exactly like a standard string but inherits all the security properties of the Axon memory model.

### Usage
```cpp
#include <axon/secure_structs.hpp>

void process_password() {
    axon::SecureString password = "my_secret_password";
    // Use like a normal string...
    // On function exit, the memory is wiped and unlocked automatically.
}
```
### Implementation Note: Type Aliasing vs. Inheritance
Axon uses a type alias (using) rather than inheritance to avoid "slicing" issues. This ensures that even if the string is passed to a function taking a std::string, the underlying allocator still handles the secure destruction.

### Requirements
+ **Include**: `<axon/secure_structs.hpp>`
+ **Namespace**: `axon::`
+ **Dependencies**: `memory.hpp`