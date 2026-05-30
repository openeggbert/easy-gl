#include "easygl/Query.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    Query::Query() = default;
    Query::~Query()
    {
        destroy();
    }

    Query::Query(Query&& other) noexcept
        : handle_(other.handle_)
        , generation_(other.generation_)
    {
        other.handle_ = 0;
        other.generation_ = 0;
    }

    Query& Query::operator=(Query&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            handle_ = other.handle_;
            generation_ = other.generation_;
            other.handle_ = 0;
            other.generation_ = 0;
        }
        return *this;
    }

    void Query::create()
    {
        if (is_created()) return;
        metagl::glGenQueries(1, &handle_);
        generation_ = metagl::GetContextGeneration();
    }

    void Query::destroy() noexcept
    {
        if (is_created())
        {
            metagl::glDeleteQueries(1, &handle_);
            handle_ = 0;
        }
    }

    void Query::begin(QueryTarget target) const
    {
        metagl::glBeginQuery(target, handle_);
    }

    void Query::end(QueryTarget target) const
    {
        metagl::glEndQuery(target);
    }

    bool Query::is_result_available() const
    {
        unsigned int available = 0;
        metagl::glGetQueryObjectuiv(handle_, metagl::QueryObjectParameter::ResultAvailable, &available);
        return available != 0;
    }

    unsigned int Query::result() const
    {
        unsigned int value = 0;
        metagl::glGetQueryObjectuiv(handle_, metagl::QueryObjectParameter::Result, &value);
        return value;
    }

    bool Query::is_created() const noexcept { return handle_ != 0; }
    void Query::reset_handle_no_gl() noexcept { handle_ = 0; generation_ = 0; }
    unsigned int Query::native_handle() const noexcept { return handle_; }
    bool Query::is_valid_for_current_generation() const noexcept
    {
        return handle_ != 0 && generation_ == metagl::GetContextGeneration();
    }
    std::uint64_t Query::creation_generation() const noexcept { return generation_; }
}
