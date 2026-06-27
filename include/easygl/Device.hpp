#pragma once

#include "easygl/Capabilities.hpp"
#include "easygl/Config.hpp"
#include "easygl/Exception.hpp"
#include "easygl/Export.hpp"
#include "easygl/Feature.hpp"
#include "easygl/Types.hpp"

#include <cstddef>
#include <span>

namespace easygl
{
    enum class ClearFlags : u32
    {
        None    = 0,
        Color   = 1 << 0,
        Depth   = 1 << 1,
        Stencil = 1 << 2
    };

    inline ClearFlags operator|(ClearFlags a, ClearFlags b)
    {
        return static_cast<ClearFlags>(static_cast<u32>(a) | static_cast<u32>(b));
    }

    class EASYGL_API Device
    {
    public:
        Device();
        explicit Device(Config config);

        void initialize(GLGetProcAddressFn loader);
        [[nodiscard]] bool is_initialized() const noexcept;

        [[nodiscard]] const Config& config() const noexcept;
        [[nodiscard]] const Capabilities& capabilities() const noexcept;

        [[nodiscard]] bool supports(Feature feature) const;
        void require(Feature feature) const;

        // ---- Clear ----
        void clear(ClearFlags flags);
        void clear_bufferfv(ClearBuffer buffer, int drawbuffer, const float* value);
        void clear_bufferiv(ClearBuffer buffer, int drawbuffer, const int* value);
        void clear_bufferuiv(ClearBuffer buffer, int drawbuffer, const unsigned int* value);
        void clear_bufferfi(int drawbuffer, float depth, int stencil);
        void set_clear_color(float r, float g, float b, float a);
        void set_clear_depth(float depth);
        void set_clear_stencil(int value);

        // ---- Viewport / scissor ----
        void set_viewport(int x, int y, int width, int height);
        void get_viewport(int& x, int& y, int& width, int& height) const;
        void set_scissor_test_enabled(bool enabled);
        void set_scissor(int x, int y, int width, int height);

        // ---- Blend ----
        void set_blend_enabled(bool enabled);
        void set_blend_func(BlendFactor sfactor, BlendFactor dfactor);
        void set_blend_func(unsigned int index, BlendFactor sfactor, BlendFactor dfactor);
        void set_blend_func_separate(BlendFactor src_rgb, BlendFactor dst_rgb,
                                     BlendFactor src_alpha, BlendFactor dst_alpha);
        void set_blend_func_separate(unsigned int index, BlendFactor src_rgb, BlendFactor dst_rgb,
                                     BlendFactor src_alpha, BlendFactor dst_alpha);
        void set_blend_equation(BlendEquation mode);
        void set_blend_equation(unsigned int index, BlendEquation mode);
        void set_blend_equation_separate(BlendEquation mode_rgb, BlendEquation mode_alpha);
        void set_blend_equation_separate(unsigned int index, BlendEquation mode_rgb, BlendEquation mode_alpha);
        void set_blend_color(float r, float g, float b, float a);

        // ---- Depth ----
        void set_depth_test_enabled(bool enabled);
        void set_depth_mask(bool enabled);
        void set_depth_func(CompareFunc func);
        void set_depth_range(float near_val, float far_val);

        // ---- Stencil ----
        void set_stencil_test_enabled(bool enabled);
        void set_stencil_func(CompareFunc func, int ref, unsigned int mask);
        void set_stencil_func_separate(CullFace face, CompareFunc func, int ref, unsigned int mask);
        void set_stencil_op(StencilOp sfail, StencilOp dpfail, StencilOp dppass);
        void set_stencil_op_separate(CullFace face, StencilOp sfail, StencilOp dpfail, StencilOp dppass);
        void set_stencil_mask(unsigned int mask);
        void set_stencil_mask_separate(CullFace face, unsigned int mask);

        // ---- Cull face ----
        void set_cull_face_enabled(bool enabled);
        void set_cull_face(CullFace face);
        void set_front_face(FrontFace face);

        // ---- Polygon / line ----
        void set_polygon_offset_fill_enabled(bool enabled);
        void set_polygon_offset(float factor, float units);
        void set_line_width(float width);

        // ---- Color mask ----
        void set_color_mask(bool r, bool g, bool b, bool a);
        void set_color_mask(unsigned int index, bool r, bool g, bool b, bool a);

        // ---- Indexed enable/disable ----
        void enable(Capability cap, unsigned int index);
        void disable(Capability cap, unsigned int index);

        // ---- Sample ----
        void set_sample_coverage(float value, bool invert);
        void set_sample_alpha_to_coverage_enabled(bool enabled);
        void set_rasterizer_discard_enabled(bool enabled);

        // ---- Pixel ----
        void set_pixel_store(PixelStoreParam pname, int value);
        void read_pixels(int x, int y, int width, int height,
                         PixelFormat format, PixelType type, void* pixels);
        void set_read_buffer(ReadBuffer src);
        void set_draw_buffers(std::span<const DrawBuffer> buffers);

        // ---- Draw calls ----
        void draw_arrays(PrimitiveType primitive, int first, int count);
        void draw_arrays_instanced(PrimitiveType primitive, int first, int count, int instance_count);
        void draw_elements(PrimitiveType primitive, int count, DataType type, const void* indices);
        void draw_elements_instanced(PrimitiveType primitive, int count, DataType type,
                                     const void* indices, int instance_count);
        void draw_arrays_indirect(PrimitiveType primitive, const void* indirect);
        void draw_elements_indirect(PrimitiveType primitive, DataType type, const void* indirect);
        void draw_range_elements(PrimitiveType primitive, unsigned int start, unsigned int end,
                                 int count, DataType type, const void* indices);

        // ---- Compute ----
        void dispatch_compute(unsigned int num_groups_x, unsigned int num_groups_y, unsigned int num_groups_z);
        void memory_barrier(MemoryBarrierMask barriers);
        void memory_barrier_by_region(MemoryBarrierMask barriers);

        // ---- Tessellation ----
        void set_patch_vertices(int count);

        // ---- Debug ----
        void set_debug_output_enabled(bool enabled);
        void set_debug_output_synchronous_enabled(bool enabled);
        void push_debug_group(DebugSource source, unsigned int id, const char* message);
        void pop_debug_group();
        void object_label(DebugObjectLabel identifier, unsigned int name, const char* label);

        // ---- State / misc ----
        void set_hint(HintTarget target, HintMode mode);
        void finish();
        void flush();
        void release_shader_compiler();
        [[nodiscard]] ErrorCode get_error();
        [[nodiscard]] GraphicsResetStatus get_graphics_reset_status();
        [[nodiscard]] bool is_enabled(Capability cap) const;
        [[nodiscard]] bool is_enabled(Capability cap, unsigned int index) const;

        // ---- Generic state getters ----
        [[nodiscard]] bool    get_boolean(GetParameter pname) const;
        [[nodiscard]] float   get_float(GetParameter pname) const;
        [[nodiscard]] int     get_integer(GetParameter pname) const;
        [[nodiscard]] i64     get_integer64(GetParameter pname) const;
        [[nodiscard]] int     get_integeri(GetParameter pname, unsigned int index) const;
        [[nodiscard]] i64     get_integer64i(GetParameter pname, unsigned int index) const;
        [[nodiscard]] bool    get_booleani(GetParameter pname, unsigned int index) const;

        // ---- Named state getters ----
        void get_blend_func(BlendFactor& src, BlendFactor& dst) const;
        [[nodiscard]] CompareFunc get_depth_func() const;
        [[nodiscard]] CullFace get_cull_face() const;
        void get_color_mask(bool& r, bool& g, bool& b, bool& a) const;

    private:
        Config config_{};
        Capabilities capabilities_{};
        GLGetProcAddressFn loader_ = nullptr;
        bool initialized_ = false;
    };
}
