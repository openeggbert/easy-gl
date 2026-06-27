#pragma once

#include "easygl/Export.hpp"
#include "easygl/RecoverableResource.hpp"
#include <metagl/ContextEvents.hpp>

#include <vector>

namespace easygl
{
    /// Central registry that tracks all RecoverableResource instances owned by
    /// an easy-gl context.  Register this as a metagl::ContextListener to
    /// receive automatic context-lost / context-restored notifications.
    ///
    /// Typical setup:
    ///   registry.register_with_meta_gl();        // once, after device init
    ///   registry.add(myTexture);                 // for each resource
    ///   // … rendering …
    ///   // On context lost  → on_context_lost()  is called automatically
    ///   // On context restored → on_context_restored() is called automatically
    class EASYGL_API ResourceRegistry final : public metagl::ContextListener
    {
    public:
        ResourceRegistry() = default;
        ~ResourceRegistry() override;

        /// Register a resource so it will be notified of context events.
        /// The caller retains ownership; the pointer must remain valid until
        /// remove() is called or the registry is destroyed.
        void add(RecoverableResource* resource);

        /// Unregister a resource.  No-op if not found.
        void remove(RecoverableResource* resource);

        /// Convenience: call metagl::AddContextListener(this).
        void register_with_meta_gl();

        /// Convenience: call metagl::RemoveContextListener(this).
        void unregister_from_meta_gl();

        // ---- metagl::ContextListener interface --------------------------------

        /// Called by meta-gl when the context is lost.
        /// Calls release_gl_handle_only() on every registered resource so that
        /// stale GL handles are discarded without issuing gl* calls.
        void OnContextLost() override;

        /// Called by meta-gl after the context has been restored.
        /// Calls recreate_gl_resource() on every registered resource.
        void OnContextRestored() override;

    private:
        std::vector<RecoverableResource*> resources_{};
        bool registered_with_meta_gl_ = false;
    };
}
