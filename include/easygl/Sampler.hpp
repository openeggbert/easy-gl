#pragma once

#include <cstdint>
#include "easygl/Export.hpp"
#include "easygl/Types.hpp"
#include "easygl/detail/GenerationTracked.hpp"
#include "easygl/detail/NonCopyable.hpp"

namespace easygl
{
    class EASYGL_API Sampler : public detail::NonCopyable, public detail::GenerationTracked
    {
    public:
        Sampler();
        ~Sampler();

        Sampler(Sampler&& other) noexcept;
        Sampler& operator=(Sampler&& other) noexcept;

        void create();
        void destroy() noexcept;

        void bind(unsigned int unit) const;
        static void unbind(unsigned int unit);

        [[nodiscard]] bool is_valid_gl_object() const;

        void set_parameter(SamplerParameter pname, int value);
        void set_parameter(SamplerParameter pname, float value);
        void get_parameter_fv(SamplerParameter pname, float* out) const;
        void get_parameter_iv(SamplerParameter pname, int* out) const;

        static Sampler create_linear_clamp();
        static Sampler create_nearest_clamp();
        static Sampler create_linear_repeat();
        static Sampler create_mipmap_linear();

    };
}
