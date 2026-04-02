#pragma once
#include <thread>
#include <barrier>
#include <functional>
#include "axon/secure_structs.hpp"


namespace axon
{
    
    template <typename CompletionFunc = std::function<void()>>
    class BarrierPool {
    private:
        size_t pool_size;
        std::vector<std::jthread> pool;
        std::barrier<CompletionFunc> sync_point;
        
        alignas(64) std::atomic<uint64_t> target_generation{0};
        alignas(64) std::atomic<uint64_t> completed_threads{0}; 
        
        std::function<void(int)> current_task;

    public:
        BarrierPool(size_t n, CompletionFunc completion = [](){}) 
            : pool_size(n), 
            sync_point(n, std::move(completion)),
            completed_threads(n) // Start initialized as "done"
        {
            pool.reserve(n);
            for (size_t i = 0; i < n; ++i) {
                pool.emplace_back([this, i](std::stop_token st) {
                    uint64_t local_generation = 0;
                    
                    while (!st.stop_requested()) {
                        bool work_ready = false;
                        for (int k = 0; k < 10000; ++k) {
                            if (target_generation.load(std::memory_order_relaxed) > local_generation) {
                                work_ready = true;
                                break;
                            }
                            #if defined(__x86_64__)
                                _mm_pause();
                            #endif   
                        }

                        if (!work_ready) {
                            while (target_generation.load(std::memory_order_acquire) <= local_generation) {
                                if (st.stop_requested()) return;
                                std::this_thread::yield(); 
                            }
                        } else {
                            std::atomic_thread_fence(std::memory_order_acquire);
                        }

                        if (current_task) current_task(static_cast<int>(i));

                        sync_point.arrive_and_wait();
                        completed_threads.fetch_add(1, std::memory_order_release);
                        local_generation++;
                    }
                });
            }
        }

        void dispatch(std::function<void(int)> task) {
            wait();

            current_task = std::move(task);
            completed_threads.store(0, std::memory_order_release);
            target_generation.fetch_add(1, std::memory_order_release);
        }

        void wait() {
            while (completed_threads.load(std::memory_order_acquire) < pool_size) {
                for (int i=0; i<10000;++i) {
                    if (completed_threads.load(std::memory_order_acquire) >= pool_size) return;
                    #if defined(__x86_64__)
                        _mm_pause();
                    #endif
                }
                if (completed_threads.load(std::memory_order_acquire) >= pool_size) return;
                std::this_thread::yield();
            }
        }
    };
} // namespace axon