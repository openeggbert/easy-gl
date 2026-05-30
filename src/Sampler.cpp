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
        : handle_(other.handle_)
        , generation_(other.generation_)
    {
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
        metagl::glGenSamplers(1, &handle_);
        generation_ = metagl::GetContextGeneration();
    }

    void Sampler::destroy() noexcept
    {
        if (is_created())
        {
            metagl::glDeleteSamplers(1, &handle_);
            handle_ = 0;
        }
    }

    void Sampler::bind(unsigned int unit) const
    {
        metagl::glBindSampler(unit, handle_);
    }

    void Sampler::unbind(unsigned int unit)
    {
        metagl::glBindSampler(unit, 0);
    }

    void Sampler::set_parameter(TextureParameter pname, int value)
    {
        metagl::glSamplerParameteri(handle_, pname, value);
    }

    void Sampler::set_parameter(TextureParameter pname, float value)
    {
        metagl::glSamplerParameterf(handle_, pname, value);
    }

    bool Sampler::is_created() const noexcept { return handle_ != 0; }
    void Sampler::reset_handle_no_gl() noexcept { handle_ = 0; generation_ = 0; }
    unsigned int Sampler::native_handle() const noexcept { return handle_; }
    bool Sampler::is_valid_for_current_generation() const noexcept
    {
        return handle_ != 0 && generation_ == metagl::GetContextGeneration();
    }
    std::uint64_t Sampler::creation_generation() const noexcept { return generation_; }
}
