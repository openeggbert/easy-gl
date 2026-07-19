#include "easygl/Sync.hpp"
#include "easygl/Exception.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    Sync::Sync() = default;
    Sync::~Sync()
    {
        destroy();
    }

    Sync::Sync(Sync&& other) noexcept
        : sync_(other.sync_)
    {
        other.sync_ = nullptr;
    }

    Sync& Sync::operator=(Sync&& other) noexcept
    {
        if (this != &other)
        {
            destroy();
            sync_ = other.sync_;
            other.sync_ = nullptr;
        }
        return *this;
    }

    void Sync::fence(SyncCondition condition)
    {
        destroy();
        // Sync objects are an ES 3.0+ feature (e.g. absent on WebGL 1); without
        // this check glFenceSync would be a null function pointer on such a
        // context, crashing on a null-pointer call instead of raising a clear
        // error. See TODO.md.
        if (!metagl::IsFunctionAvailable("glFenceSync"))
            throw UnsupportedFeatureException("Sync objects are not supported by the current context.");
        sync_ = metagl::glFenceSync(condition, metagl::SyncFlag::None);
    }

    void Sync::destroy() noexcept
    {
        if (sync_)
        {
            metagl::glDeleteSync(sync_);
            sync_ = nullptr;
        }
    }

    SyncWaitResult Sync::client_wait(SyncFlushMask flags, std::uint64_t timeout_ns)
    {
        return metagl::glClientWaitSync(sync_, flags, timeout_ns);
    }

    void Sync::server_wait(std::uint64_t timeout_ns)
    {
        metagl::glWaitSync(sync_, metagl::SyncFlag::None, timeout_ns);
    }

    int Sync::get_parameter(SyncParameter pname) const
    {
        GLint value = 0;
        GLsizei length = 0;
        metagl::glGetSynciv(sync_, pname, 1, &length, &value);
        return value;
    }

    bool Sync::is_created() const noexcept { return sync_ != nullptr; }
    void Sync::reset_handle_no_gl() noexcept { sync_ = nullptr; }
    metagl::GLsync Sync::native_handle() const noexcept { return sync_; }
}
