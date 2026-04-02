#include <string>
#include <thread>
#include <atomic>
#include <iostream>
#include <bit>
#include <concepts>
#include <immintrin.h>
#include "memory.hpp"

namespace axon {

    /* Ring Buffer Single Producer Single Consumer
    *  Buffer size should be a power of 2
    *  Lock free Ring buffer
    *  Initialize with n blocks of data type T the class allocated on initialization
    *  Call getNext to acquire the block, and finish to release it
    */
    template <typename T = char>
    requires std::is_trivially_copyable_v<T>
    class RingBufferSPSC {
        private:
        std::vector<T, AlignedAllocator<T, 64>> buffer;
        size_t num_blocks;
        size_t buffer_size;
        alignas(64) std::atomic<size_t> wrindex{0};
        alignas(64) std::atomic<size_t> rindex{0};

        public:

        inline void operator=(RingBufferSPSC* r) = delete;

        inline size_t getWriteIndex() { return wrindex.load() & (num_blocks-1); }
        inline size_t getReadIndex() { return rindex.load() & (num_blocks-1); }
        inline size_t getBlockSize() { return sizeof(T); }

        inline void finishWrite() noexcept { 
            wrindex.fetch_add(1, std::memory_order_release);
        }
        inline void finishRead() noexcept { 
            rindex.fetch_add(1, std::memory_order_release);
        }

        inline bool isFull() noexcept { 
            return wrindex.load() - rindex.load() == num_blocks;
        }

        inline void hybridWait(std::atomic<size_t>& value, size_t curr_val) {
            for (int i = 0; i < 10000; ++i) {
                if (value.load(std::memory_order_relaxed) != curr_val) return;
                #if defined(__x86_64__)
                    _mm_pause();
                #endif
            }
            while (value.load(std::memory_order_relaxed) == curr_val) {
                std::this_thread::yield();
            }
        }

        inline T* getNextRead() {
            size_t wr = wrindex.load(std::memory_order_acquire);
            while (rindex.load(std::memory_order_relaxed) >= wr) {
                hybridWait(wrindex, wr);
                wr = wrindex.load(std::memory_order_acquire);
            }
            return &buffer[rindex.load() & (num_blocks-1)];
        }

        inline T* getNextWrite() {
            size_t rd = rindex.load(std::memory_order_acquire);
            while (wrindex.load(std::memory_order_relaxed) - rd == num_blocks) {
                hybridWait(rindex, rd);
                rd = rindex.load(std::memory_order_acquire);
            }
            return &buffer[wrindex.load(std::memory_order_relaxed) & (num_blocks-1)];
        }

        RingBufferSPSC(size_t blocks) : 
            num_blocks(std::bit_ceil(blocks)), 
            buffer(num_blocks), 
            buffer_size(buffer.size())
            {}
    };
} // namespace axon