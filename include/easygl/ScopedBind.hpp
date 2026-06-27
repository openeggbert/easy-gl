#pragma once
#include <functional>
#include <utility>

namespace easygl
{
    class ScopedBind
    {
    public:
        template<typename BindFn, typename UnbindFn>
        ScopedBind(BindFn&& bind_fn, UnbindFn&& unbind_fn)
            : unbind_(std::forward<UnbindFn>(unbind_fn))
        {
            std::forward<BindFn>(bind_fn)();
        }

        ~ScopedBind() { unbind_(); }

        ScopedBind(const ScopedBind&) = delete;
        ScopedBind& operator=(const ScopedBind&) = delete;
        ScopedBind(ScopedBind&&) = delete;
        ScopedBind& operator=(ScopedBind&&) = delete;

    private:
        std::function<void()> unbind_;
    };
}
