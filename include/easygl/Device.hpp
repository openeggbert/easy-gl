#pragma once

#include "easygl/Capabilities.hpp"
#include "easygl/Config.hpp"
#include "easygl/Exception.hpp"
#include "easygl/Export.hpp"
#include "easygl/Feature.hpp"
#include "easygl/Types.hpp"
#include <metagl/metagl.hpp>

namespace easygl
{
    using PrimitiveType = metagl::PrimitiveType;
    using BlendFactor   = metagl::BlendFactor;
    using CompareFunc   = metagl::CompareFunc;
    using CullFace      = metagl::CullFace;
    using FrontFace     = metagl::FrontFace;

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

        // Commands
        void clear(ClearFlags flags);
        void set_clear_color(float r, float g, float b, float a);
        void set_viewport(int x, int y, int width, int height);
        void get_viewport(int& x, int& y, int& width, int& height) const;
        void set_blend_enabled(bool enabled);
        void set_blend_func(BlendFactor sfactor, BlendFactor dfactor);
        /**
         * @brief Enables or disables the depth test.
         */
        void set_depth_test_enabled(bool enabled);
        /**
         * @brief Enables or disables writing to the depth buffer.
         */
        void set_depth_mask(bool enabled);
        /**
         * @brief Sets the depth comparison function.
         */
        void set_depth_func(CompareFunc func);
        /**
         * @brief Sets the depth value used by depth-buffer clears.
         */
        void set_clear_depth(float depth);
        /**
         * @brief Enables or disables face culling.
         */
        void set_cull_face_enabled(bool enabled);
        /**
         * @brief Sets which face is culled.
         */
        void set_cull_face(CullFace face);
        /**
         * @brief Sets the winding order considered "front".
         */
        void set_front_face(FrontFace face);
        void draw_arrays(PrimitiveType primitive, int first, int count);
        void draw_elements(PrimitiveType primitive, int count, DataType type, const void* indices);

    private:
        Config config_{};
        Capabilities capabilities_{};
        GLGetProcAddressFn loader_ = nullptr;
        bool initialized_ = false;
    };
}
