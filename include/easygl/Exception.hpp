#pragma once

#include <stdexcept>
#include <string>
#include "easygl/Export.hpp"

namespace easygl
{
    class EASYGL_API Exception : public std::runtime_error
    {
    public:
        explicit Exception(const std::string& message)
            : std::runtime_error(message) {}
    };

    class EASYGL_API UnsupportedFeatureException : public Exception
    {
    public:
        explicit UnsupportedFeatureException(const std::string& message)
            : Exception(message) {}
    };
}