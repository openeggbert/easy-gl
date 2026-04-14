#pragma once

#include "easygl/Export.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class Shader;

    class EASYGL_API Program : public detail::NonCopyable
    {
    public:
        Program();
        ~Program();

        Program(Program&& other) noexcept;
        Program& operator=(Program&& other) noexcept;

        void create();
        void destroy() noexcept;
        void attach(const Shader& shader);
        void link();
        void use() const;

        [[nodiscard]] bool is_linked() const noexcept;
        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;

    private:
        unsigned int handle_ = 0;
        bool linked_ = false;
    };
}