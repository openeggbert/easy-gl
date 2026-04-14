#pragma once

#include "easygl/Export.hpp"

namespace easygl
{
    class Shader;

    class EASYGL_API Program
    {
    public:
        Program();
        ~Program();

        void attach(const Shader& shader);
        void link();
        void use() const;

        [[nodiscard]] bool is_linked() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;

    private:
        unsigned int handle_ = 0;
        bool linked_ = false;
    };
}