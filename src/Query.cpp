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
    {
        handle_ = other.handle_;
        generation_ = other.generation_;
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
        metagl::QueryId qid{};
        metagl::glGenQueries(1, &qid);
        handle_ = qid.value;
        generation_ = metagl::GetContextGeneration();
    }

    void Query::destroy() noexcept
    {
        if (is_created())
        {
            metagl::QueryId qid{handle_};
            metagl::glDeleteQueries(1, &qid);
            handle_ = 0;
        }
    }

    void Query::begin(QueryTarget target) const
    {
        metagl::glBeginQuery(target, metagl::QueryId{handle_});
    }

    void Query::end(QueryTarget target) const
    {
        metagl::glEndQuery(target);
    }

    bool Query::is_result_available() const
    {
        unsigned int available = 0;
        metagl::glGetQueryObjectuiv(metagl::QueryId{handle_}, metagl::QueryObjectParameter::ResultAvailable, &available);
        return available != 0;
    }

    unsigned int Query::result() const
    {
        unsigned int value = 0;
        metagl::glGetQueryObjectuiv(metagl::QueryId{handle_}, metagl::QueryObjectParameter::Result, &value);
        return value;
    }

}
