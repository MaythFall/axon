#pragma once
#include <cstddef>
#include <cstdlib>
#include <new> 
#include <utility>
#include <string>
#include "axon/errors.hpp"

#if defined(_WIN32) || defined(_WIN64)
    #define WIN32_LEAN_AND_MEAN
    #include <WinSock2.h>
    #include <Windows.h>
    #include <malloc.h> 
    #define __windows__
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    #include <sys/mman.h>
    #include <string.h>
#endif

namespace axon {
    
    //Force wipe memory unoptimizable
    inline static void secureWipe(void *data, size_t size) noexcept {
        #ifdef __windows__
            SecureZeroMemory(data, size);
        #elif defined(__linux__) || defined(__FreeBSD__) || defined(__OpenBSD__)
            explicit_bzero(data, size);
        #elif defined(__STDC_LIB_EXT1__) || (__cplusplus >= 202302L)
            memset_explicit(data, 0, size);
        #else
            volatile unsigned char* p = static_cast<volatile unsigned char*>(data);
            while (size--) *p++ = 0;
        #endif
    }

    inline static bool lockMemory(void* addr, size_t len) noexcept {
        #ifdef __windows__
            return VirtualLock(addr, len) != 0;
        #elif defined(__linux__) || defined(__unix__)
            return mlock(addr, len) == 0;
        #else
            return false;
        #endif
    }

    inline static void unlockMemory(void* addr, size_t len) noexcept {
        #ifdef __windows__
            VirtualUnlock(addr, len);
        #elif defined(__linux__) || defined(__unix__)
            munlock(addr, len);
        #endif
    }
    
    
    
    template <typename U, size_t Align = 64, bool Secure = true>
    struct AlignedAllocator {
        using value_type = U;

        U* allocate(size_t n) {
            if (n == 0) return nullptr;
            size_t bytes = (n * sizeof(U) + Align - 1) & ~(Align - 1);
            
            void* ptr = nullptr;
            #ifdef __windows__
                ptr = _aligned_malloc(bytes, Align);
            #else
                ptr = std::aligned_alloc(Align, bytes);
            #endif

            if (!ptr) throw std::bad_alloc();

            if constexpr (Secure) {
                if(!lockMemory(ptr, bytes)) {
                    #ifdef AXON_THROW_ON_LOCK_FAIL
                        throw memlock_failure("");
                    #endif
                } 
            }
            return static_cast<U*>(ptr);
        }

        void deallocate(U* p, size_t n) noexcept {
            if (!p) return;
            size_t bytes = (n * sizeof(U) + Align - 1) & ~(Align - 1);

            if constexpr (Secure) {
                secureWipe(p, bytes);
                unlockMemory(p, bytes);
            }

            #ifdef __windows__
                _aligned_free(p);
            #else
                std::free(p);
            #endif
        }

        template <typename V> 
        struct rebind { using other = AlignedAllocator<V, Align>; };

        bool operator==(const AlignedAllocator&) const noexcept { return true; }
        bool operator!=(const AlignedAllocator&) const noexcept { return false; }
    };

} // namespace axon