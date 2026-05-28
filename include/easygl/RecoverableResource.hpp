#pragma once

#include "easygl/Export.hpp"

namespace easygl
{
    /// Base class for any GL resource that can be recreated after context loss.
    ///
    /// Derived classes should store all CPU-side data required to recreate the
    /// GPU resource (e.g. texture pixels, shader source, buffer contents).
    class EASYGL_API RecoverableResource
    {
    public:
        virtual ~RecoverableResource() = default;

        /// Drop the GPU handle without calling any gl* functions.
        /// Called by ResourceRegistry when the context has been lost.
        /// The resource must keep its CPU-side data intact so that
        /// recreate_gl_resource() can restore it later.
        virtual void release_gl_handle_only() = 0;

        /// Recreate the GPU resource using the stored CPU-side data.
        /// Called by ResourceRegistry after a context has been restored and
        /// new function pointers have been loaded.
        virtual void recreate_gl_resource() = 0;
    };
}
