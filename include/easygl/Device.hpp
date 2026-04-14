#pragma once

#include "easygl/Capabilities.hpp"
#include "easygl/Config.hpp"
#include "easygl/Exception.hpp"
#include "easygl/Export.hpp"
#include "easygl/Feature.hpp"
#include "easygl/Types.hpp"

namespace easygl
{
    enum class PrimitiveType
    {
        Triangles,
        TriangleStrip,
        TriangleFan,
        Lines,
        LineStrip,
        LineLoop,
        Points
    };

    enum class ClearFlags : u32
    {
        None = 0,
        Color = 1 << 0,
        Depth = 1 << 1,
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
        void draw_arrays(PrimitiveType primitive, int first, int count);

    private:
        Config config_{};
        Capabilities capabilities_{};
        GLGetProcAddressFn loader_ = nullptr;
        bool initialized_ = false;
    };
}