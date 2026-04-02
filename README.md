# Axon Non-Standard Library
---
**Axon** is a header-only C++ library of secure, high-performance primitives for systems programming — aligned memory allocation, secure data structures with guaranteed wipe-on-destruction, a lock-free SPSC ring buffer, and portable 128-bit integer support.

## Installation
**Axon** is comprised of only headers *currently* in order to make ports as easy as possible.  
Simply copy the include folder and then include any of the files with `#include "axon/axon.hpp"` or `#include <axon/axon.hpp>` if using CMake.

## Files
Currently **Axon** consists of five files and a general include file. The files can be included separately however it is reccomended to bring the whole library as there are some internal dependencies.  

| File | Description | 
|------|--------------------|
| [`memory.hpp`](docs/memory_management.md) | Aligned allocator, secure wipe, memory locking |
| [`secure_structs.hpp`](docs/secure_structs.md) | `SecureBuffer`, `SecureString` with RAII wipe |
| [`ring_buffer.hpp`](docs/concurrency.md) | Lock-free single-producer single-consumer ring buffer |
| [`types.hpp`](docs/data_types.md)| Portable uint128_t with compiler intrinsic detection |
| [`errors.hpp`](docs/error_management.md) | Custom exception types |

## The Philosophy 
The name **Axon** (from the Ancient Greek *ἄξων*, or "axis/nerve fiber") reflects the library's role as a core high-speed conduit for data. In biological systems, an axon is the specialized fiber that transmits electrical impulses efficiently across distances; in software, this library provides the high-performance "nerve fibers" that connect secure, low-latency backends.

## Requirements
- **Language:** C++20 or higher
- **Platforms:** Linux, Windows (MSVC), and macOS (the latter two have not been tested yet but should work)
- **Compiler:** GCC 10+, Clang 10+, or MSVC 19.28+

## Documentation
For full API details, implementation notes, and performance benchmarks, 
please refer to the [Full Documentation Index](docs/api_index.md).

## License
Open-source under the MIT License. See `LICENSE` for details.
