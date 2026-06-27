#include "easygl/Sampler.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    Sampler::Sampler() = default;
    Sampler::~Sampler()
    {
        destroy();
    }

    Sampler::Sampler(Sampler&& other) noexcept
    {
        handle_ = other.handle_;
        generation_ = other.generation_;
        other.handle_ = 0;
        other.generation_ = 0;
    }

    Sampler& Sampler::operator=(Sampler&& other) noexcept
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

    void Sampler::create()
    {
        if (is_created()) return;
        metagl::SamplerId sid{};
        metagl::glGenSamplers(1, &sid);
        handle_ = sid.value;
        generation_ = metagl::GetContextGeneration();
    }

    void Sampler::destroy() noexcept
    {
        if (is_created())
        {
            metagl::SamplerId sid{handle_};
            metagl::glDeleteSamplers(1, &sid);
            handle_ = 0;
        }
    }

    void Sampler::bind(unsigned int unit) const
    {
        metagl::glBindSampler(unit, metagl::SamplerId{handle_});
    }

    void Sampler::unbind(unsigned int unit)
    {
        metagl::glBindSampler(unit, metagl::SamplerId{0});
    }

    void Sampler::set_parameter(SamplerParameter pname, int value)
    {
        metagl::glSamplerParameteri(metagl::SamplerId{handle_}, pname, value);
    }

    void Sampler::set_parameter(SamplerParameter pname, float value)
    {
        metagl::glSamplerParameterf(metagl::SamplerId{handle_}, pname, value);
    }

    Sampler Sampler::create_linear_clamp()
    {
        Sampler s; s.create();
        s.set_parameter(SamplerParameter::MinFilter, static_cast<int>(metagl::TextureMinFilter::Linear));
        s.set_parameter(SamplerParameter::MagFilter, static_cast<int>(metagl::TextureMagFilter::Linear));
        s.set_parameter(SamplerParameter::WrapS,     static_cast<int>(metagl::TextureWrapMode::ClampToEdge));
        s.set_parameter(SamplerParameter::WrapT,     static_cast<int>(metagl::TextureWrapMode::ClampToEdge));
        return s;
    }

    Sampler Sampler::create_nearest_clamp()
    {
        Sampler s; s.create();
        s.set_parameter(SamplerParameter::MinFilter, static_cast<int>(metagl::TextureMinFilter::Nearest));
        s.set_parameter(SamplerParameter::MagFilter, static_cast<int>(metagl::TextureMagFilter::Nearest));
        s.set_parameter(SamplerParameter::WrapS,     static_cast<int>(metagl::TextureWrapMode::ClampToEdge));
        s.set_parameter(SamplerParameter::WrapT,     static_cast<int>(metagl::TextureWrapMode::ClampToEdge));
        return s;
    }

    Sampler Sampler::create_linear_repeat()
    {
        Sampler s; s.create();
        s.set_parameter(SamplerParameter::MinFilter, static_cast<int>(metagl::TextureMinFilter::Linear));
        s.set_parameter(SamplerParameter::MagFilter, static_cast<int>(metagl::TextureMagFilter::Linear));
        s.set_parameter(SamplerParameter::WrapS,     static_cast<int>(metagl::TextureWrapMode::Repeat));
        s.set_parameter(SamplerParameter::WrapT,     static_cast<int>(metagl::TextureWrapMode::Repeat));
        return s;
    }

    Sampler Sampler::create_mipmap_linear()
    {
        Sampler s; s.create();
        s.set_parameter(SamplerParameter::MinFilter, static_cast<int>(metagl::TextureMinFilter::LinearMipmapLinear));
        s.set_parameter(SamplerParameter::MagFilter, static_cast<int>(metagl::TextureMagFilter::Linear));
        s.set_parameter(SamplerParameter::WrapS,     static_cast<int>(metagl::TextureWrapMode::Repeat));
        s.set_parameter(SamplerParameter::WrapT,     static_cast<int>(metagl::TextureWrapMode::Repeat));
        return s;
    }

    void Sampler::get_parameter_fv(SamplerParameter pname, float* out) const
    {
        metagl::glGetSamplerParameterfv(metagl::SamplerId{handle_}, pname, out);
    }

    void Sampler::get_parameter_iv(SamplerParameter pname, int* out) const
    {
        metagl::glGetSamplerParameteriv(metagl::SamplerId{handle_}, pname, out);
    }

}
namespace easygl { bool Sampler::is_valid_gl_object() const { return is_created() && metagl::glIsSampler(metagl::SamplerId{handle_}); } }
