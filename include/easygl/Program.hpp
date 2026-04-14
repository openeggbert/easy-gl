#pragma once
#include <string>
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
        void detach(const Shader& shader);
        void link();
        void use() const;

        [[nodiscard]] std::string info_log() const;
        [[nodiscard]] int uniform_location(const std::string& name) const;
        void set_uniform(int location, int value);
        void set_uniform(int location, float value);
        void set_uniform(int location, float x, float y, float z);
        void set_uniform_matrix4(int location, const float* data, bool transpose = false);

        [[nodiscard]] bool is_linked() const noexcept;
        [[nodiscard]] bool is_created() const noexcept;
        [[nodiscard]] unsigned int native_handle() const noexcept;

    private:
        unsigned int handle_ = 0;
        bool linked_ = false;
    };
}