# Concurrency
This document outlines the high-performance concurrency primitives provided by **Axon**. The library focuses on lock-free architectures, cache-line awareness, and minimizing kernel-level context switching.

---

## RingBufferSPSC
The **`RingBufferSPSC<T>`** is a high-performance, Single-Producer Single-Consumer circular queue. It is the primary mechanism for low-latency data transfer between two threads.

### Performance Architecture
* **Lock-Free Design**: Uses atomic memory primitives with std::memory_order_acquire and std::memory_order_release semantics. This avoids the overhead of mutexes and OS-level locking.
* **False Sharing Prevention**: The write and read indices are padded with alignas(64). This ensures that the producer and consumer do not compete for the same CPU cache line, preventing performance degradation.
* **Bitwise Indexing**: Buffer sizes are automatically rounded up to the nearest power of two using std::bit_ceil. This allows Axon to replace the expensive modulo operator (%) with a single-cycle bitwise AND (&).
* **Hybrid Waiting**: Axon uses a multi-stage waiting strategy to balance CPU usage and latency.

### API Reference

#### Initialization

```cpp
// Size will be rounded up to the nearest power of two (e.g., 1000 -> 1024)
axon::RingBufferSPSC<MyData> queue(1024);
```

#### Producer Methods
* **T* getNextWrite()**: Returns a pointer to the next available block in the buffer. If the buffer is full, it enters a hybridWait.
* **void finishWrite()**: Finalizes the write operation and makes the data visible to the consumer.

#### Consumer Methods
* **T* getNextRead()**: Returns a pointer to the next available data block. If the buffer is empty, it enters a hybridWait.
* **void finishRead()**: Marks the block as processed, allowing the producer to reuse the space.

---

## Hybrid Waiting Strategy
Axon utilizes a specialized **hybridWait** function to manage thread contention efficiently. Instead of immediately yielding to the OS (which causes an expensive context switch), the thread follows this sequence:

1.  **Spinning**: The thread polls the atomic variable in a tight loop for 10,000 iterations.
2.  **Hardware Hinting**: Inside the spin-loop, it executes `_mm_pause()` (on `x86_64`). This tells the CPU that the thread is in a spin-wait, reducing power consumption and preventing pipeline stalls.
3.  **Yielding**: If the condition is still not met after 10,000 cycles, the thread calls `std::this_thread::yield()`, allowing the OS to schedule other tasks.

### Example Loop

```cpp
// Producer Thread
auto* slot = queue.getNextWrite();
*slot = generate_data();
queue.finishWrite();

// Consumer Thread
auto* data = queue.getNextRead();
process(data);
queue.finishRead();
```

### Requirements
* **Include**: `<axon/ring_buffer.hpp>`
* **Namespace**: `axon::`
* **Standard**: `C++20` (for `<bit>` and `std::bit_ceil`)