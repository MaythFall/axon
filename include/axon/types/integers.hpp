#pragma once
#include <cstdint>
#include <concepts>

namespace axon {

    #if defined(__SIZEOF_INT128__)
        using uint128_t = unsigned __int128;

    #else
        class uint128_t {
            uint64_t num[2] = {0};

            public:
            uint128_t() = default;
            uint128_t(uint64_t low, uint64_t high) : num{low, high} {}

            template<std::unsigned_integral T>
            explicit uint128_t(T val) : num{static_cast<uint64_t>(val), 0} {}

            uint128_t operator+(const uint128_t& other) const {
                uint64_t l = num[0] + other.num[0];
                uint64_t h = num[1] + other.num[1];
                if (l < num[0]) h += 1;
                return uint128_t(l, h);
            }

            uint128_t& operator+=(const uint128_t& other) {
                uint64_t prev = num[0];
                num[0] += other.num[0];
                if (num[0] < prev) num[1] += 1;
                num[1] += other.num[1];
                return *this;
            }

            uint128_t& operator++() {
                if (++num[0] == 0) ++num[1];
                return *this;
            }

            uint128_t operator++(int) {
                uint128_t tmp = *this;
                ++(*this);
                return tmp;
            }

            bool operator==(const uint128_t& other) const {
                return num[0] == other.num[0] && num[1] == other.num[1];
            }

            bool operator<(const uint128_t& other) const {
                return num[1] < other.num[1] ||
                    (num[1] == other.num[1] && num[0] < other.num[0]);
            }

            bool operator>(const uint128_t& other)  const { return other < *this; }
            bool operator<=(const uint128_t& other) const { return !(other < *this); }
            bool operator>=(const uint128_t& other) const { return !(*this < other); }

            const uint64_t& low()  const { return num[0]; }
            const uint64_t& high() const { return num[1]; }
            uint64_t* data() { return num; }
            const uint64_t* data() const { return num; }
        };

    #endif // __SIZEOF_INT128__


    inline uint64_t low(uint128_t v) {
        #if defined(__SIZEOF_INT128__)
            return static_cast<uint64_t>(v);
        #else
            return v.low();
        #endif
    }

    inline uint64_t high(uint128_t v) {
        #if defined(__SIZEOF_INT128__)
            return static_cast<uint64_t>(v >> 64);
        #else
            return v.high();
        #endif
    }

} // namespace axon