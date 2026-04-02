#pragma once
#include <stdexcept>
#include <cerrno>

namespace axon
{
    class memlock_failure : public std::runtime_error{
        public:
            using std::runtime_error::runtime_error;
    };

    class network_error : public std::runtime_error{
        public:
            using std::runtime_error::runtime_error;
    };

    class busy_thread_error : public std::runtime_error{
        public:
            using std::runtime_error::runtime_error;
    };

    class io_error : public std::runtime_error{
        public:
            using std::runtime_error::runtime_error;
    };


} // namespace axon
