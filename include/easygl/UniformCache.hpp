#pragma once
#include <string>
#include <string_view>
#include <unordered_map>
#include "easygl/Export.hpp"

namespace easygl
{
    class Program;

    class EASYGL_API UniformCache
    {
    public:
        explicit UniformCache(const Program& program);

        [[nodiscard]] int operator[](std::string_view name);

        void invalidate();

    private:
        const Program* program_;
        std::unordered_map<std::string, int> cache_;
    };
}
