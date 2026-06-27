#include "easygl/Device.hpp"
#include <metagl/metagl.hpp>
#include <utility>
#include <stdexcept>
#include <vector>

namespace easygl
{
    Device::Device() : Device(Config{})
    {
    }

    Device::Device(Config config)
        : config_(config)
        , initialized_(false)
    {
    }

    void Device::initialize(GLGetProcAddressFn loader)
    {
        if (initialized_) return;

        if (loader == nullptr)
            throw Exception("GL loader callback is null.");

        loader_ = loader;

        if (!metagl::Initialize(loader_))
            throw Exception("Failed to bootstrap GL functions.");

        const char* vendor_ptr     = reinterpret_cast<const char*>(metagl::glGetString(metagl::StringName::Vendor));
        const char* renderer_ptr   = reinterpret_cast<const char*>(metagl::glGetString(metagl::StringName::Renderer));
        const char* version_ptr    = reinterpret_cast<const char*>(metagl::glGetString(metagl::StringName::Version));
        const char* sl_version_ptr = reinterpret_cast<const char*>(metagl::glGetString(metagl::StringName::ShadingLanguageVersion));

        std::string vendor                = vendor_ptr     ? vendor_ptr     : "";
        std::string renderer              = renderer_ptr   ? renderer_ptr   : "";
        std::string version_string        = version_ptr    ? version_ptr    : "";
        std::string shading_language_version = sl_version_ptr ? sl_version_ptr : "";

        ApiKind api = ApiKind::OpenGL;
        if (version_string.find("OpenGL ES") != std::string::npos)
            api = ApiKind::OpenGLES;

        int major = 0;
        int minor = 0;

        metagl::glGetIntegerv(metagl::GetParameter::MajorVersion, &major);
        metagl::glGetIntegerv(metagl::GetParameter::MinorVersion, &minor);

        if (major == 0)
        {
            const char* v = version_string.c_str();
            if (api == ApiKind::OpenGLES && version_string.length() > 10) v += 10;
            if (v && *v >= '0' && *v <= '9')
            {
                major = *v - '0';
                if (*(v + 1) == '.' && *(v + 2) >= '0' && *(v + 2) <= '9')
                    minor = *(v + 2) - '0';
            }
        }

        std::vector<std::string> extensions;
        if (major >= 3)
        {
            int num_extensions = 0;
            metagl::glGetIntegerv(metagl::GetParameter::NumExtensions, &num_extensions);
            for (int i = 0; i < num_extensions; ++i)
            {
                const char* ext = reinterpret_cast<const char*>(
                    metagl::glGetStringi(metagl::StringName::Extensions, static_cast<unsigned int>(i)));
                if (ext) extensions.push_back(ext);
            }
        }
        else
        {
            const char* ext_ptr = reinterpret_cast<const char*>(metagl::glGetString(metagl::StringName::Extensions));
            if (ext_ptr)
            {
                std::string s(ext_ptr);
                std::string delimiter = " ";
                size_t pos = 0;
                while ((pos = s.find(delimiter)) != std::string::npos)
                {
                    std::string token = s.substr(0, pos);
                    if (!token.empty()) extensions.push_back(token);
                    s.erase(0, pos + delimiter.length());
                }
                if (!s.empty()) extensions.push_back(s);
            }
        }

        ContextInfo info;
        info.api = api;
        info.major = major;
        info.minor = minor;
        info.vendor = vendor;
        info.renderer = renderer;
        info.version_string = version_string;
        info.shading_language_version = shading_language_version;
        info.extensions = std::move(extensions);

        capabilities_.set_context_info(std::move(info));
        capabilities_.detect_common_features();

        if (!capabilities_.supports(Feature::VertexArrayObject)) throw Exception("VertexArrayObject support is required.");
        if (!capabilities_.supports(Feature::Shader))            throw Exception("Shader support is required.");
        if (!capabilities_.supports(Feature::Program))           throw Exception("Program support is required.");
        if (!capabilities_.supports(Feature::Buffer))            throw Exception("Buffer support is required.");
        if (!capabilities_.supports(Feature::BasicRendering))    throw Exception("Basic rendering support is required.");

        int max_texture_size = 0;
        metagl::glGetIntegerv(metagl::GetParameter::MaxTextureSize, &max_texture_size);
        capabilities_.set_limit("max_texture_size", max_texture_size);

        initialized_ = true;
    }

    bool Device::is_initialized() const noexcept { return initialized_; }
    const Config& Device::config() const noexcept { return config_; }
    const Capabilities& Device::capabilities() const noexcept { return capabilities_; }
    bool Device::supports(Feature feature) const { return capabilities_.supports(feature); }

    void Device::require(Feature feature) const
    {
        if (!supports(feature) && config_.throw_on_missing_feature)
            throw UnsupportedFeatureException("Requested feature is not supported by the current device.");
    }

    // ---- Clear ----

    void Device::clear(ClearFlags flags)
    {
        metagl::ClearBufferBit mask = static_cast<metagl::ClearBufferBit>(0);
        if ((static_cast<u32>(flags) & static_cast<u32>(ClearFlags::Color))   != 0) mask = mask | metagl::ClearBufferBit::Color;
        if ((static_cast<u32>(flags) & static_cast<u32>(ClearFlags::Depth))   != 0) mask = mask | metagl::ClearBufferBit::Depth;
        if ((static_cast<u32>(flags) & static_cast<u32>(ClearFlags::Stencil)) != 0) mask = mask | metagl::ClearBufferBit::Stencil;
        metagl::glClear(mask);
    }

    void Device::clear_bufferfv(ClearBuffer buffer, int drawbuffer, const float* value)
    {
        metagl::glClearBufferfv(buffer, drawbuffer, value);
    }

    void Device::clear_bufferiv(ClearBuffer buffer, int drawbuffer, const int* value)
    {
        metagl::glClearBufferiv(buffer, drawbuffer, value);
    }

    void Device::clear_bufferuiv(ClearBuffer buffer, int drawbuffer, const unsigned int* value)
    {
        metagl::glClearBufferuiv(buffer, drawbuffer, value);
    }

    void Device::clear_bufferfi(int drawbuffer, float depth, int stencil)
    {
        metagl::glClearBufferfi(metagl::ClearBuffer::Depth, drawbuffer, depth, stencil);
    }

    void Device::set_clear_color(float r, float g, float b, float a)
    {
        metagl::glClearColor(r, g, b, a);
    }

    void Device::set_clear_depth(float depth)
    {
        metagl::glClearDepthf(depth);
    }

    void Device::set_clear_stencil(int value)
    {
        metagl::glClearStencil(value);
    }

    // ---- Viewport / scissor ----

    void Device::set_viewport(int x, int y, int width, int height)
    {
        metagl::glViewport(x, y, width, height);
    }

    void Device::get_viewport(int& x, int& y, int& width, int& height) const
    {
        int viewport[4];
        metagl::glGetIntegerv(metagl::GetParameter::Viewport, viewport);
        x = viewport[0]; y = viewport[1]; width = viewport[2]; height = viewport[3];
    }

    void Device::set_scissor_test_enabled(bool enabled)
    {
        if (enabled) metagl::glEnable(metagl::Capability::ScissorTest);
        else         metagl::glDisable(metagl::Capability::ScissorTest);
    }

    void Device::set_scissor(int x, int y, int width, int height)
    {
        metagl::glScissor(x, y, width, height);
    }

    // ---- Blend ----

    void Device::set_blend_enabled(bool enabled)
    {
        if (enabled) metagl::glEnable(metagl::Capability::Blend);
        else         metagl::glDisable(metagl::Capability::Blend);
    }

    void Device::set_blend_func(BlendFactor sfactor, BlendFactor dfactor)
    {
        metagl::glBlendFunc(sfactor, dfactor);
    }

    void Device::set_blend_func_separate(BlendFactor src_rgb, BlendFactor dst_rgb,
                                          BlendFactor src_alpha, BlendFactor dst_alpha)
    {
        metagl::glBlendFuncSeparate(src_rgb, dst_rgb, src_alpha, dst_alpha);
    }

    void Device::set_blend_equation(BlendEquation mode)
    {
        metagl::glBlendEquation(mode);
    }

    void Device::set_blend_equation_separate(BlendEquation mode_rgb, BlendEquation mode_alpha)
    {
        metagl::glBlendEquationSeparate(mode_rgb, mode_alpha);
    }

    void Device::set_blend_color(float r, float g, float b, float a)
    {
        metagl::glBlendColor(r, g, b, a);
    }

    void Device::set_blend_func(unsigned int index, BlendFactor sfactor, BlendFactor dfactor)
    {
        metagl::glBlendFunci(index, sfactor, dfactor);
    }

    void Device::set_blend_func_separate(unsigned int index, BlendFactor src_rgb, BlendFactor dst_rgb,
                                          BlendFactor src_alpha, BlendFactor dst_alpha)
    {
        metagl::glBlendFuncSeparatei(index, src_rgb, dst_rgb, src_alpha, dst_alpha);
    }

    void Device::set_blend_equation(unsigned int index, BlendEquation mode)
    {
        metagl::glBlendEquationi(index, mode);
    }

    void Device::set_blend_equation_separate(unsigned int index, BlendEquation mode_rgb, BlendEquation mode_alpha)
    {
        metagl::glBlendEquationSeparatei(index, mode_rgb, mode_alpha);
    }

    // ---- Depth ----

    void Device::set_depth_test_enabled(bool enabled)
    {
        if (enabled) metagl::glEnable(metagl::Capability::DepthTest);
        else         metagl::glDisable(metagl::Capability::DepthTest);
    }

    void Device::set_depth_mask(bool enabled)
    {
        metagl::glDepthMask(enabled ? 1 : 0);
    }

    void Device::set_depth_func(CompareFunc func)
    {
        metagl::glDepthFunc(func);
    }

    void Device::set_depth_range(float near_val, float far_val)
    {
        metagl::glDepthRangef(near_val, far_val);
    }

    // ---- Stencil ----

    void Device::set_stencil_test_enabled(bool enabled)
    {
        if (enabled) metagl::glEnable(metagl::Capability::StencilTest);
        else         metagl::glDisable(metagl::Capability::StencilTest);
    }

    void Device::set_stencil_func(CompareFunc func, int ref, unsigned int mask)
    {
        metagl::glStencilFunc(func, ref, mask);
    }

    void Device::set_stencil_func_separate(CullFace face, CompareFunc func, int ref, unsigned int mask)
    {
        metagl::glStencilFuncSeparate(face, func, ref, mask);
    }

    void Device::set_stencil_op(StencilOp sfail, StencilOp dpfail, StencilOp dppass)
    {
        metagl::glStencilOp(sfail, dpfail, dppass);
    }

    void Device::set_stencil_op_separate(CullFace face, StencilOp sfail, StencilOp dpfail, StencilOp dppass)
    {
        metagl::glStencilOpSeparate(face, sfail, dpfail, dppass);
    }

    void Device::set_stencil_mask(unsigned int mask)
    {
        metagl::glStencilMask(mask);
    }

    void Device::set_stencil_mask_separate(CullFace face, unsigned int mask)
    {
        metagl::glStencilMaskSeparate(face, mask);
    }

    // ---- Cull face ----

    void Device::set_cull_face_enabled(bool enabled)
    {
        if (enabled) metagl::glEnable(metagl::Capability::CullFace);
        else         metagl::glDisable(metagl::Capability::CullFace);
    }

    void Device::set_cull_face(CullFace face) { metagl::glCullFace(face); }
    void Device::set_front_face(FrontFace face) { metagl::glFrontFace(face); }

    // ---- Polygon / line ----

    void Device::set_polygon_offset_fill_enabled(bool enabled)
    {
        if (enabled) metagl::glEnable(metagl::Capability::PolygonOffsetFill);
        else         metagl::glDisable(metagl::Capability::PolygonOffsetFill);
    }

    void Device::set_polygon_offset(float factor, float units)
    {
        metagl::glPolygonOffset(factor, units);
    }

    void Device::set_line_width(float width)
    {
        metagl::glLineWidth(width);
    }

    // ---- Color mask ----

    void Device::set_color_mask(bool r, bool g, bool b, bool a)
    {
        metagl::glColorMask(r ? 1 : 0, g ? 1 : 0, b ? 1 : 0, a ? 1 : 0);
    }

    void Device::set_color_mask(unsigned int index, bool r, bool g, bool b, bool a)
    {
        metagl::glColorMaski(index, r ? GL_TRUE : GL_FALSE, g ? GL_TRUE : GL_FALSE,
                              b ? GL_TRUE : GL_FALSE, a ? GL_TRUE : GL_FALSE);
    }

    void Device::enable(Capability cap, unsigned int index)
    {
        metagl::glEnablei(cap, index);
    }

    void Device::disable(Capability cap, unsigned int index)
    {
        metagl::glDisablei(cap, index);
    }

    // ---- Sample ----

    void Device::set_sample_coverage(float value, bool invert)
    {
        metagl::glSampleCoverage(value, invert ? 1 : 0);
    }

    void Device::set_sample_alpha_to_coverage_enabled(bool enabled)
    {
        if (enabled) metagl::glEnable(metagl::Capability::SampleAlphaToCoverage);
        else         metagl::glDisable(metagl::Capability::SampleAlphaToCoverage);
    }

    void Device::set_rasterizer_discard_enabled(bool enabled)
    {
        if (enabled) metagl::glEnable(metagl::Capability::RasterizerDiscard);
        else         metagl::glDisable(metagl::Capability::RasterizerDiscard);
    }

    void Device::set_sample_mask(unsigned int mask_number, unsigned int mask)
    {
        metagl::glSampleMaski(mask_number, static_cast<metagl::SampleMaskValue>(mask));
    }

    void Device::set_min_sample_shading(float value)
    {
        metagl::glMinSampleShading(value);
    }

    void Device::blend_barrier()
    {
        metagl::glBlendBarrier();
    }

    // ---- Pixel ----

    void Device::set_pixel_store(PixelStoreParam pname, int value)
    {
        metagl::glPixelStorei(pname, value);
    }

    void Device::read_pixels(int x, int y, int width, int height,
                              PixelFormat format, PixelType type, void* pixels)
    {
        metagl::glReadPixels(x, y, width, height, format, type, pixels);
    }

    void Device::set_read_buffer(ReadBuffer src)
    {
        metagl::glReadBuffer(src);
    }

    void Device::set_draw_buffers(std::span<const DrawBuffer> buffers)
    {
        metagl::glDrawBuffers(static_cast<GLsizei>(buffers.size()), buffers.data());
    }

    // ---- Draw calls ----

    void Device::draw_arrays(PrimitiveType primitive, int first, int count)
    {
        metagl::glDrawArrays(primitive, first, count);
    }

    void Device::draw_arrays_instanced(PrimitiveType primitive, int first, int count, int instance_count)
    {
        metagl::glDrawArraysInstanced(primitive, first, count, instance_count);
    }

    void Device::draw_elements(PrimitiveType primitive, int count, DataType type, const void* indices)
    {
        metagl::glDrawElements(primitive, count, type, indices);
    }

    void Device::draw_elements_instanced(PrimitiveType primitive, int count, DataType type,
                                          const void* indices, int instance_count)
    {
        metagl::glDrawElementsInstanced(primitive, count, type, indices, instance_count);
    }

    void Device::draw_arrays_indirect(PrimitiveType primitive, const void* indirect)
    {
        metagl::glDrawArraysIndirect(primitive, indirect);
    }

    void Device::draw_elements_indirect(PrimitiveType primitive, DataType type, const void* indirect)
    {
        metagl::glDrawElementsIndirect(primitive, type, indirect);
    }

    void Device::draw_range_elements(PrimitiveType primitive, unsigned int start, unsigned int end,
                                     int count, DataType type, const void* indices)
    {
        metagl::glDrawRangeElements(primitive, start, end, static_cast<GLsizei>(count), type, indices);
    }

    // ---- Compute ----

    void Device::dispatch_compute(unsigned int num_groups_x, unsigned int num_groups_y, unsigned int num_groups_z)
    {
        metagl::glDispatchCompute(num_groups_x, num_groups_y, num_groups_z);
    }

    void Device::memory_barrier(MemoryBarrierMask barriers)
    {
        metagl::glMemoryBarrier(barriers);
    }

    void Device::memory_barrier_by_region(MemoryBarrierMask barriers)
    {
        metagl::glMemoryBarrierByRegion(barriers);
    }

    // ---- Tessellation ----

    void Device::set_patch_vertices(int count)
    {
        metagl::glPatchParameteri(metagl::TessellationParameter::PatchVertices, count);
    }

    // ---- Debug ----

    void Device::set_debug_output_enabled(bool enabled)
    {
        if (enabled) metagl::glEnable(metagl::Capability::DebugOutput);
        else         metagl::glDisable(metagl::Capability::DebugOutput);
    }

    void Device::set_debug_output_synchronous_enabled(bool enabled)
    {
        if (enabled) metagl::glEnable(metagl::Capability::DebugOutputSynchronous);
        else         metagl::glDisable(metagl::Capability::DebugOutputSynchronous);
    }

    void Device::push_debug_group(DebugSource source, unsigned int id, const char* message)
    {
        metagl::glPushDebugGroup(source, id, -1, message);
    }

    void Device::pop_debug_group()
    {
        metagl::glPopDebugGroup();
    }

    void Device::object_label(DebugObjectLabel identifier, unsigned int name, const char* label)
    {
        metagl::glObjectLabel(identifier, name, -1, label);
    }

    // ---- State / misc ----

    void Device::set_hint(HintTarget target, HintMode mode)
    {
        metagl::glHint(target, mode);
    }

    void Device::finish()
    {
        metagl::glFinish();
    }

    void Device::flush()
    {
        metagl::glFlush();
    }

    void Device::release_shader_compiler()
    {
        metagl::glReleaseShaderCompiler();
    }

    ErrorCode Device::get_error()
    {
        return metagl::glGetError();
    }

    GraphicsResetStatus Device::get_graphics_reset_status()
    {
        return metagl::glGetGraphicsResetStatus();
    }

    bool Device::is_enabled(Capability cap) const
    {
        return metagl::glIsEnabled(cap);
    }

    bool Device::is_enabled(Capability cap, unsigned int index) const
    {
        return metagl::glIsEnabledi(cap, index);
    }

    bool Device::get_boolean(GetParameter pname) const
    {
        GLboolean v = GL_FALSE;
        metagl::glGetBooleanv(pname, &v);
        return v != GL_FALSE;
    }

    float Device::get_float(GetParameter pname) const
    {
        GLfloat v = 0.0f;
        metagl::glGetFloatv(pname, &v);
        return v;
    }

    int Device::get_integer(GetParameter pname) const
    {
        GLint v = 0;
        metagl::glGetIntegerv(pname, &v);
        return v;
    }

    i64 Device::get_integer64(GetParameter pname) const
    {
        GLint64 v = 0;
        metagl::glGetInteger64v(pname, &v);
        return static_cast<i64>(v);
    }

    int Device::get_integeri(GetParameter pname, unsigned int index) const
    {
        GLint v = 0;
        metagl::glGetIntegeri_v(pname, index, &v);
        return v;
    }

    i64 Device::get_integer64i(GetParameter pname, unsigned int index) const
    {
        GLint64 v = 0;
        metagl::glGetInteger64i_v(pname, index, &v);
        return static_cast<i64>(v);
    }

    bool Device::get_booleani(GetParameter pname, unsigned int index) const
    {
        GLboolean v = GL_FALSE;
        metagl::glGetBooleani_v(pname, index, &v);
        return v != GL_FALSE;
    }

    void Device::get_blend_func(BlendFactor& src, BlendFactor& dst) const
    {
        GLint s = 0, d = 0;
        metagl::glGetIntegerv(metagl::GetParameter::BlendSrcRgb, &s);
        metagl::glGetIntegerv(metagl::GetParameter::BlendDstRgb, &d);
        src = static_cast<BlendFactor>(s);
        dst = static_cast<BlendFactor>(d);
    }

    CompareFunc Device::get_depth_func() const
    {
        GLint v = 0;
        metagl::glGetIntegerv(metagl::GetParameter::DepthFunc, &v);
        return static_cast<CompareFunc>(v);
    }

    CullFace Device::get_cull_face() const
    {
        GLint v = 0;
        metagl::glGetIntegerv(metagl::GetParameter::CullFaceMode, &v);
        return static_cast<CullFace>(v);
    }

    void Device::get_color_mask(bool& r, bool& g, bool& b, bool& a) const
    {
        GLboolean mask[4] = {};
        metagl::glGetBooleanv(metagl::GetParameter::ColorWritemask, mask);
        r = mask[0] != GL_FALSE;
        g = mask[1] != GL_FALSE;
        b = mask[2] != GL_FALSE;
        a = mask[3] != GL_FALSE;
    }
}
