#pragma once
#include <cstdint>
#include <vector>
#include <array>
#include <stdexcept>
#include <algorithm>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <chrono>
#include <concepts>
#include "memory.hpp"

#if defined(_WIN32) || defined(_WIN64)
    #define WIN32_LEAN_AND_MEAN
    #include <WinSock2.h>
    #include <Windows.h>
    #define __windows__
#elif defined(__linux__) || defined(__unix__) || defined(__APPLE__)
    #include <sys/mman.h>
    #include <string.h>
#endif

#include <immintrin.h>


namespace axon {

    //RAII wrapper for secure data storage; does not allow copies
    template <typename T = uint8_t, size_t Align = 32>
    requires std::is_trivially_copyable_v<T>
    class SecureBuffer {
        private:
        std::vector<T, AlignedAllocator<T, Align>> data_;
        bool is_locked = false;

        public:
        explicit SecureBuffer(size_t size) : data_(size) {}
        SecureBuffer() = default;

        ~SecureBuffer() = default; 

        SecureBuffer(const SecureBuffer&) = delete;
        SecureBuffer& operator=(const SecureBuffer&) = delete;

        SecureBuffer(SecureBuffer&&) noexcept = default;
        SecureBuffer& operator=(SecureBuffer&&) noexcept = default;

        T* data() { return data_.data(); }
        const T* data() const { return data_.data(); }
        size_t size() const { return data_.size(); }
        void resize(size_t count) { data_.resize(count); }
        void resize(size_t count, const T& value) { data_.resize(count, value); }
        T& operator[] (size_t pos) {return data_[pos];}
        const T& operator[] (size_t pos) const {return data_[pos];}
        bool empty() const { return data_.empty(); }
        
        template<typename... Args>
        void emplace_back(Args&&... args) { 
            data_.emplace_back(std::forward<Args>(args)...); 
        }
    };

    using SecureString = std::basic_string<char, std::char_traits<char>, AlignedAllocator<char, 64>>;

} // namespace axon