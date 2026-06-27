#include "easygl/UniformCache.hpp"
#include "easygl/Program.hpp"

namespace easygl
{
    UniformCache::UniformCache(const Program& program)
        : program_(&program)
    {
    }

    int UniformCache::operator[](std::string_view name)
    {
        auto it = cache_.find(std::string(name));
        if (it != cache_.end())
            return it->second;
        const int loc = program_->uniform_location(std::string(name));
        cache_.emplace(std::string(name), loc);
        return loc;
    }

    void UniformCache::invalidate()
    {
        cache_.clear();
    }
}
