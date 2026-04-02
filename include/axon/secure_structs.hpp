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
#include <tuple>
#include "axon/memory/memory.hpp"

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

    template <int Alignment = 64, bool Secure = true, typename... Types>
    struct ArrayContainer {
        std::tuple<std::vector<Types, axon::AlignedAllocator<Types, Alignment, Secure>>...> data;

        ArrayContainer(size_t initial_capacity) {
            std::apply([initial_capacity](auto&... vecs) {
                (vecs.reserve(initial_capacity), ...);
            }, data);
        }

        template <typename... Us>
        void add_entry(Us&&... args) {
            static_assert(sizeof...(Us) == sizeof...(Types), "Wrong number of arguments for SoA entry.");
            add_entry_impl(std::make_index_sequence<sizeof...(Types)>{}, std::forward<Us>(args)...);
        }

        void add_entry(std::tuple<Types...> entry) {
            std::apply([this](auto&&... args) {
                this->add_entry(std::forward<decltype(args)>(args)...);
            }, entry);
        }

        template <size_t Index>
        auto& get_array() {
            return std::get<Index>(data);
        }

        void remove_entry(size_t index) {
            remove_impl(std::make_index_sequence<sizeof...(Types)>{}, index, false);
        }

        void swap_pop_entry(size_t index) {
            remove_impl(std::make_index_sequence<sizeof...(Types)>{}, index, true);
        }

    private:
        template <size_t... Is, typename... Us>
        void add_entry_impl(std::index_sequence<Is...>, Us&&... args) {
            (std::get<Is>(data).emplace_back(std::forward<Us>(args)), ...);
        }

        template <size_t... Is>
        void remove_impl(std::index_sequence<Is...>, size_t index, bool fast) {
            if (fast) {
                ([&] {
                    auto& vec = std::get<Is>(data);
                    if (index < vec.size() - 1) {
                        vec[index] = std::move(vec.back());
                    }
                    vec.pop_back();
                }(), ...);
            } else {
                (std::get<Is>(data).erase(std::get<Is>(data).begin() + index), ...);
            }
        }

    };

} // namespace axon