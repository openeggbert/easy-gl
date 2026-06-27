#pragma once

#include <cstdint>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class EASYGL_API Sync : public detail::NonCopyable
    {
    public:
        Sync();
        ~Sync();

        Sync(Sync&& other) noexcept;
        Sync& operator=(Sync&& other) noexcept;

        void fence(SyncCondition condition = SyncCondition::GpuCommandsComplete);
        void destroy() noexcept;

        [[nodiscard]] SyncWaitResult client_wait(SyncFlushMask flags, std::uint64_t timeout_ns);
        void server_wait(std::uint64_t timeout_ns);

        [[nodiscard]] int get_parameter(SyncParameter pname) const;
        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] metagl::GLsync native_handle() const noexcept;

        void reset_handle_no_gl() noexcept;

    private:
        metagl::GLsync sync_ = nullptr;
    };
}
