#include "easygl/Renderbuffer.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    Renderbuffer::Renderbuffer() = default;
    Renderbuffer::~Renderbuffer()
    {
        destroy();
    }

    Renderbuffer::Renderbuffer(Renderbuffer&& other) noexcept
    {
        handle_ = other.handle_;
        generation_ = other.generation_;
        other.handle_ = 0;
        other.generation_ = 0;
    }

    Renderbuffer& Renderbuffer::operator=(Renderbuffer&& other) noexcept
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

    void Renderbuffer::create()
    {
        if (is_created()) return;
        metagl::RenderbufferId rid{};
        metagl::glGenRenderbuffers(1, &rid);
        handle_ = rid.value;
        generation_ = metagl::GetContextGeneration();
    }

    void Renderbuffer::destroy() noexcept
    {
        if (is_created())
        {
            metagl::RenderbufferId rid{handle_};
            metagl::glDeleteRenderbuffers(1, &rid);
            handle_ = 0;
        }
    }

    void Renderbuffer::bind() const
    {
        metagl::glBindRenderbuffer(metagl::RenderbufferTarget::Renderbuffer, metagl::RenderbufferId{handle_});
    }

    void Renderbuffer::unbind()
    {
        metagl::glBindRenderbuffer(metagl::RenderbufferTarget::Renderbuffer, metagl::RenderbufferId{0});
    }

    void Renderbuffer::set_storage(InternalFormat internal_format, int width, int height)
    {
        metagl::glRenderbufferStorage(metagl::RenderbufferTarget::Renderbuffer,
                                      internal_format, width, height);
    }

    void Renderbuffer::set_storage_multisample(int samples, InternalFormat internal_format, int width, int height)
    {
        metagl::glRenderbufferStorageMultisample(metagl::RenderbufferTarget::Renderbuffer,
                                                  samples, internal_format, width, height);
    }

    int Renderbuffer::get_parameter(RenderbufferParameter pname) const
    {
        int value = 0;
        metagl::glGetRenderbufferParameteriv(metagl::RenderbufferTarget::Renderbuffer, pname, &value);
        return value;
    }

}
namespace easygl { bool Renderbuffer::is_valid_gl_object() const { return is_created() && metagl::glIsRenderbuffer(metagl::RenderbufferId{handle_}); } }
