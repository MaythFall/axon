#pragma once
#include <stdexcept>

namespace axon
{
    class memlock_failure : public std::runtime_error{
        public:
            using std::runtime_error::runtime_error;
    };


} // namespace axon
