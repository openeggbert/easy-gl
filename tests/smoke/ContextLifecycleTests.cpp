/// Tests for meta-gl context lifecycle and easy-gl resource restoration.
///
/// No real GPU context is required; a fake function-pointer loader is used.

#include <easygl/easygl.hpp>
#include <metagl/metagl.hpp>

#include <cassert>
#include <cstring>
#include <iostream>
#include <string>

// ---------------------------------------------------------------------------
// Fake loader helpers
// ---------------------------------------------------------------------------
namespace
{
    /// Minimal stub loader that satisfies metagl::Initialize().
    /// Provides working glGetString / glGetIntegerv so that context-info
    /// detection works correctly.
    static void* fake_loader(const char* name)
    {
        std::string n(name);

        if (n == "glGetString")
        {
            return reinterpret_cast<void*>(+[](unsigned int /*pname*/) -> const unsigned char*
            {
                // Return a version string that resembles "OpenGL ES 3.0"
                return reinterpret_cast<const unsigned char*>("OpenGL ES 3.0");
            });
        }
        if (n == "glGetStringi")
        {
            return reinterpret_cast<void*>(+[](unsigned int /*name*/, unsigned int /*index*/) -> const unsigned char*
            {
                return reinterpret_cast<const unsigned char*>("");
            });
        }
        if (n == "glGetIntegerv")
        {
            return reinterpret_cast<void*>(+[](unsigned int pname, int* data)
            {
                if (pname == 0x821B) *data = 3;   // GL_MAJOR_VERSION
                if (pname == 0x821C) *data = 0;   // GL_MINOR_VERSION
                if (pname == 0x821D) *data = 0;   // GL_NUM_EXTENSIONS
                if (pname == 0x0D33) *data = 1024; // GL_MAX_TEXTURE_SIZE
            });
        }
        // Return a non-null but non-callable sentinel for all other functions.
        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xDEADBEEF));
    }

    /// Loader where glGetString returns nullptr for everything (simulates a
    /// stripped-down context for capability tests with explicit strings).
    static void* minimal_loader(const char* name)
    {
        std::string n(name);

        if (n == "glGetString")
        {
            return reinterpret_cast<void*>(+[](unsigned int pname) -> const unsigned char*
            {
                // 0x1F02 = GL_VERSION, return stub ES 2.0 string
                if (pname == 0x1F02) return reinterpret_cast<const unsigned char*>("OpenGL ES 2.0 stub");
                return reinterpret_cast<const unsigned char*>("");
            });
        }
        if (n == "glGetStringi")
        {
            return reinterpret_cast<void*>(+[](unsigned int, unsigned int) -> const unsigned char*
            {
                return reinterpret_cast<const unsigned char*>("");
            });
        }
        if (n == "glGetIntegerv")
        {
            return reinterpret_cast<void*>(+[](unsigned int pname, int* data)
            {
                if (pname == 0x821B) *data = 2;
                if (pname == 0x821C) *data = 0;
                if (pname == 0x821D) *data = 0;
                if (pname == 0x0D33) *data = 512;
            });
        }
        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xDEADBEEF));
    }

    /// Set of GLES 3.0+ function names that are NOT part of GLES 2.0 core.
    /// Used by the gles2_only_loader to return nullptr for higher-level funcs.
    static bool is_gles3_only(const std::string& n)
    {
        // A representative (not exhaustive) sample of GLES 3.0+ functions.
        static const char* gles3_fns[] = {
            "glGenVertexArrays", "glDeleteVertexArrays", "glBindVertexArray", "glIsVertexArray",
            "glGetStringi", "glGetInteger64v", "glGetIntegeri_v", "glGetInteger64i_v",
            "glGetBooleani_v", "glBindBufferBase", "glBindBufferRange", "glCopyBufferSubData",
            "glMapBufferRange", "glFlushMappedBufferRange", "glUnmapBuffer",
            "glGetBufferParameteri64v", "glGetBufferPointerv",
            "glVertexAttribIPointer", "glVertexAttribDivisor",
            "glVertexAttribI4i", "glVertexAttribI4ui", "glVertexAttribI4iv", "glVertexAttribI4uiv",
            "glGetVertexAttribIiv", "glGetVertexAttribIuiv",
            "glDrawArraysInstanced", "glDrawRangeElements", "glDrawElementsInstanced",
            "glDrawBuffers", "glReadBuffer",
            "glClearBufferfv", "glClearBufferiv", "glClearBufferuiv", "glClearBufferfi",
            "glTexImage3D", "glTexSubImage3D", "glTexStorage2D", "glTexStorage3D",
            "glCompressedTexImage3D", "glCompressedTexSubImage3D", "glCopyTexSubImage3D",
            "glTexParameterIiv", "glTexParameterIuiv", "glGetTexParameterIiv", "glGetTexParameterIuiv",
            "glGetTexLevelParameteriv", "glGetTexLevelParameterfv",
            "glGenSamplers", "glDeleteSamplers", "glBindSampler", "glIsSampler",
            "glSamplerParameterf", "glSamplerParameteri", "glSamplerParameterfv",
            "glSamplerParameteriv", "glGetSamplerParameterfv", "glGetSamplerParameteriv",
            "glFramebufferTextureLayer", "glBlitFramebuffer",
            "glInvalidateFramebuffer", "glInvalidateSubFramebuffer",
            "glRenderbufferStorageMultisample",
            "glGenTransformFeedbacks", "glDeleteTransformFeedbacks", "glBindTransformFeedback",
            "glFenceSync", "glDeleteSync", "glIsSync", "glClientWaitSync", "glWaitSync", "glGetSynciv",
            "glGenQueries", "glDeleteQueries", "glIsQuery", "glBeginQuery", "glEndQuery",
            "glGetQueryiv", "glGetQueryObjectuiv",
            "glUniform1ui", "glUniform2ui", "glUniform3ui", "glUniform4ui",
            "glGetUniformuiv", "glGetnUniformfv", "glGetnUniformiv", "glGetnUniformuiv",
            "glGetUniformBlockIndex", "glGetActiveUniformBlockiv", "glGetActiveUniformBlockName",
            "glUniformBlockBinding", "glGetUniformIndices", "glGetActiveUniformsiv",
            "glGetProgramBinary", "glProgramBinary", "glProgramParameteri", "glGetFragDataLocation",
            // GLES 3.1+
            "glDispatchCompute", "glDispatchComputeIndirect", "glMemoryBarrier",
            "glMemoryBarrierByRegion", "glGenProgramPipelines", "glDeleteProgramPipelines",
            "glBindProgramPipeline", "glUseProgramStages", "glActiveShaderProgram",
            "glCreateShaderProgramv", "glValidateProgramPipeline",
            "glVertexAttribFormat", "glVertexAttribIFormat", "glVertexAttribBinding",
            "glBindVertexBuffer", "glVertexBindingDivisor",
            "glDrawArraysIndirect", "glDrawElementsIndirect",
            "glTexStorage2DMultisample", "glGetInternalformativ", "glBindImageTexture",
            "glFramebufferParameteri", "glGetFramebufferParameteriv",
            "glGetProgramInterfaceiv", "glGetProgramResourceIndex", "glGetProgramResourceName",
            "glGetProgramResourceiv", "glGetProgramResourceLocation",
            "glSampleMaski", "glMinSampleShading", "glGetPointerv",
            // GLES 3.2+
            "glEnablei", "glDisablei", "glIsEnabledi",
            "glBlendFunci", "glBlendFuncSeparatei", "glBlendEquationi", "glBlendEquationSeparatei",
            "glColorMaski", "glBlendBarrier",
            "glCopyImageSubData", "glTexBuffer", "glTexBufferRange",
            "glTexStorage3DMultisample", "glGetMultisamplefv",
            "glSamplerParameterIiv", "glSamplerParameterIuiv",
            "glGetSamplerParameterIiv", "glGetSamplerParameterIuiv",
            "glFramebufferTexture",
            "glPatchParameteri", "glPrimitiveBoundingBox",
            "glDebugMessageCallback", "glDebugMessageControl", "glDebugMessageInsert",
            "glGetDebugMessageLog", "glPushDebugGroup", "glPopDebugGroup",
            "glObjectLabel", "glObjectPtrLabel", "glGetObjectLabel", "glGetObjectPtrLabel",
            "glDrawElementsBaseVertex", "glDrawRangeElementsBaseVertex",
            "glDrawElementsInstancedBaseVertex", "glDrawRangeElementsBaseVertex",
            "glProgramUniform1f", "glProgramUniform2f", "glProgramUniform3f", "glProgramUniform4f",
            "glReadnPixels", "glGetGraphicsResetStatus",
            nullptr
        };
        for (int i = 0; gles3_fns[i] != nullptr; ++i)
            if (n == gles3_fns[i]) return true;
        return false;
    }

    /// GLES 2.0-only loader: returns working stubs only for GLES 2.0 core,
    /// and nullptr for any GLES 3.0+ function.
    static void* gles2_only_loader(const char* name)
    {
        std::string n(name);

        if (is_gles3_only(n))
            return nullptr;

        if (n == "glGetString")
        {
            return reinterpret_cast<void*>(+[](unsigned int pname) -> const unsigned char*
            {
                if (pname == 0x1F02) return reinterpret_cast<const unsigned char*>("OpenGL ES 2.0");
                return reinterpret_cast<const unsigned char*>("");
            });
        }
        if (n == "glGetIntegerv")
        {
            return reinterpret_cast<void*>(+[](unsigned int pname, int* data)
            {
                if (pname == 0x821B) *data = 2;
                if (pname == 0x821C) *data = 0;
                if (pname == 0x821D) *data = 0;
                if (pname == 0x0D33) *data = 512;
            });
        }
        // All GLES 2.0 functions get a non-null sentinel.
        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xDEADBEEF));
    }

    /// GLES 3.0 loader: returns working stubs for GLES 2.0 + GLES 3.0 core,
    /// nullptr for GLES 3.1/3.2-only functions.
    static bool is_gles31_only(const std::string& n)
    {
        static const char* gles31_fns[] = {
            "glDispatchCompute", "glDispatchComputeIndirect", "glMemoryBarrier",
            "glMemoryBarrierByRegion", "glGenProgramPipelines", "glDeleteProgramPipelines",
            "glBindProgramPipeline", "glUseProgramStages", "glActiveShaderProgram",
            "glCreateShaderProgramv", "glValidateProgramPipeline",
            "glVertexAttribFormat", "glVertexAttribIFormat", "glVertexAttribBinding",
            "glBindVertexBuffer", "glVertexBindingDivisor",
            "glDrawArraysIndirect", "glDrawElementsIndirect",
            // NOTE: glGetInternalformativ is GLES 3.0 core (see
            // meta-gl's OpenGL_ES.md reference table and its
            // gles30_required_names in RequiredFunctions.inc), not
            // 3.1-only, so it must stay available for this GLES-3.0-only
            // loader; only glTexStorage2DMultisample/glBindImageTexture
            // are genuinely 3.1+.
            "glTexStorage2DMultisample", "glBindImageTexture",
            "glFramebufferParameteri", "glGetFramebufferParameteriv",
            "glGetProgramInterfaceiv", "glGetProgramResourceIndex", "glGetProgramResourceName",
            "glGetProgramResourceiv", "glGetProgramResourceLocation",
            "glEnablei", "glDisablei", "glIsEnabledi",
            "glBlendFunci", "glBlendFuncSeparatei", "glBlendEquationi", "glBlendEquationSeparatei",
            "glColorMaski", "glBlendBarrier",
            "glCopyImageSubData", "glTexBuffer", "glTexBufferRange",
            "glTexStorage3DMultisample", "glGetMultisamplefv",
            "glSamplerParameterIiv", "glSamplerParameterIuiv",
            "glGetSamplerParameterIiv", "glGetSamplerParameterIuiv",
            "glFramebufferTexture",
            "glPatchParameteri", "glPrimitiveBoundingBox",
            "glDebugMessageCallback", "glDebugMessageControl", "glDebugMessageInsert",
            "glGetDebugMessageLog", "glPushDebugGroup", "glPopDebugGroup",
            "glObjectLabel", "glObjectPtrLabel", "glGetObjectLabel", "glGetObjectPtrLabel",
            "glReadnPixels", "glGetGraphicsResetStatus",
            nullptr
        };
        for (int i = 0; gles31_fns[i] != nullptr; ++i)
            if (n == gles31_fns[i]) return true;
        return false;
    }

    static void* gles3_only_loader(const char* name)
    {
        std::string n(name);

        if (is_gles31_only(n))
            return nullptr;

        if (n == "glGetString")
        {
            return reinterpret_cast<void*>(+[](unsigned int pname) -> const unsigned char*
            {
                if (pname == 0x1F02) return reinterpret_cast<const unsigned char*>("OpenGL ES 3.0");
                return reinterpret_cast<const unsigned char*>("");
            });
        }
        if (n == "glGetStringi")
        {
            return reinterpret_cast<void*>(+[](unsigned int, unsigned int) -> const unsigned char*
            {
                return reinterpret_cast<const unsigned char*>("");
            });
        }
        if (n == "glGetIntegerv")
        {
            return reinterpret_cast<void*>(+[](unsigned int pname, int* data)
            {
                if (pname == 0x821B) *data = 3;
                if (pname == 0x821C) *data = 0;
                if (pname == 0x821D) *data = 0;
                if (pname == 0x0D33) *data = 1024;
            });
        }
        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xDEADBEEF));
    }

    /// Loader that is missing glBindAttribLocation — one of the previously-missing
    /// required GLES 2.0 functions now added to gles2_minimum_loaded().
    /// Initialize() must return false for this loader.
    static void* gles2_missing_required_loader(const char* name)
    {
        std::string n(name);
        // Withhold two required GLES 2.0 core functions to verify both are checked:
        // glDrawArrays (was already required) and glBindAttribLocation (newly required).
        if (n == "glDrawArrays" || n == "glBindAttribLocation") return nullptr;
        return gles2_only_loader(name);
    }
}

// ---------------------------------------------------------------------------
// Test: context generation increments after each Initialize()
// ---------------------------------------------------------------------------
static void test_generation_increments()
{
    // Reset by calling Initialize once.
    bool ok = metagl::Initialize(fake_loader);
    assert(ok && "Initialize should succeed with fake_loader");

    const std::uint64_t gen1 = metagl::GetContextGeneration();
    assert(gen1 >= 1u && "Generation must be at least 1 after first init");

    // Call again (simulates a context restore).
    ok = metagl::Initialize(fake_loader);
    assert(ok);
    const std::uint64_t gen2 = metagl::GetContextGeneration();
    assert(gen2 == gen1 + 1u && "Generation must increment on each Initialize");

    std::cout << "  [PASS] test_generation_increments (gen1=" << gen1 << " gen2=" << gen2 << ")\n";
}

// ---------------------------------------------------------------------------
// Test: context lost state prevents rendering
// ---------------------------------------------------------------------------
static void test_context_lost_status()
{
    metagl::Initialize(fake_loader);
    assert(metagl::GetContextStatus() == metagl::ContextStatus::Current);
    assert(!metagl::IsContextLost());

    metagl::MarkContextLost();
    assert(metagl::GetContextStatus() == metagl::ContextStatus::Lost);
    assert(metagl::IsContextLost());

    // Restore: after a new Initialize, status should be Current again.
    metagl::Initialize(fake_loader);
    assert(metagl::GetContextStatus() == metagl::ContextStatus::Current);
    assert(!metagl::IsContextLost());

    std::cout << "  [PASS] test_context_lost_status\n";
}

// ---------------------------------------------------------------------------
// Test: IsFunctionAvailable returns expected values
// ---------------------------------------------------------------------------
static void test_is_function_available()
{
    metagl::Initialize(fake_loader);

    // glGetString is provided by fake_loader → should be available.
    assert(metagl::IsFunctionAvailable("glGetString"));
    // glGetIntegerv is provided → should be available.
    assert(metagl::IsFunctionAvailable("glGetIntegerv"));
    // A completely unknown function name should not be available.
    assert(!metagl::IsFunctionAvailable("glNonExistentFunctionXYZ123"));

    std::cout << "  [PASS] test_is_function_available\n";
}

// ---------------------------------------------------------------------------
// Test: capabilities can be initialised with mocked GL strings
// ---------------------------------------------------------------------------
static void test_capabilities_from_mocked_strings()
{
    metagl::Initialize(fake_loader);

    const metagl::Capabilities& caps = metagl::GetCapabilities();
    // fake_loader returns "OpenGL ES 3.0" for GL_VERSION.
    assert(caps.gles20 && "GLES 2.0 flag should be set for a 3.0 context");
    assert(caps.gles30 && "GLES 3.0 flag should be set");
    assert(!caps.gles31 && "GLES 3.1 flag should NOT be set for a 3.0 context");

    // Use minimal_loader for an ES 2.0 context.
    metagl::Initialize(minimal_loader);
    const metagl::Capabilities& caps2 = metagl::GetCapabilities();
    assert(caps2.gles20 && "GLES 2.0 flag should be set");
    assert(!caps2.gles30 && "GLES 3.0 flag should NOT be set for a 2.0 context");

    std::cout << "  [PASS] test_capabilities_from_mocked_strings\n";
}

// ---------------------------------------------------------------------------
// Test: NotifyContextLost / NotifyContextRestored dispatch to listeners
// ---------------------------------------------------------------------------
static void test_context_events()
{
    metagl::Initialize(fake_loader);

    int lost_count = 0;
    int restored_count = 0;

    struct Listener : metagl::ContextListener
    {
        int& lost;
        int& restored;
        Listener(int& l, int& r) : lost(l), restored(r) {}
        void OnContextLost()     override { ++lost; }
        void OnContextRestored() override { ++restored; }
    };

    Listener listener(lost_count, restored_count);
    metagl::AddContextListener(&listener);

    metagl::NotifyContextLost();
    assert(lost_count == 1);
    assert(metagl::IsContextLost());

    metagl::Initialize(fake_loader);
    metagl::NotifyContextRestored();
    assert(restored_count == 1);

    metagl::RemoveContextListener(&listener);

    // After removal, no further callbacks.
    metagl::NotifyContextLost();
    assert(lost_count == 1 && "Should not have incremented after removal");
    // Restore for subsequent tests.
    metagl::Initialize(fake_loader);

    std::cout << "  [PASS] test_context_events\n";
}

// ---------------------------------------------------------------------------
// Test: ResourceRegistry calls release_gl_handle_only on context lost
// ---------------------------------------------------------------------------
static void test_resource_registry_context_lost()
{
    metagl::Initialize(fake_loader);

    int release_count = 0;
    int recreate_count = 0;

    struct FakeResource : easygl::RecoverableResource
    {
        int& released;
        int& recreated;
        FakeResource(int& rel, int& rec) : released(rel), recreated(rec) {}
        void release_gl_handle_only() override { ++released; }
        void recreate_gl_resource()   override { ++recreated; }
    };

    FakeResource r1(release_count, recreate_count);
    FakeResource r2(release_count, recreate_count);

    easygl::ResourceRegistry registry;
    registry.add(&r1);
    registry.add(&r2);

    // Simulate context lost.
    registry.OnContextLost();
    assert(release_count == 2 && "Both resources should have been released");
    assert(recreate_count == 0 && "No recreation on context lost");

    // Simulate context restored.
    registry.OnContextRestored();
    assert(recreate_count == 2 && "Both resources should have been recreated");

    std::cout << "  [PASS] test_resource_registry_context_lost\n";
}

// ---------------------------------------------------------------------------
// Test: ResourceRegistry is wired to meta-gl context events
// ---------------------------------------------------------------------------
static void test_resource_registry_wired_to_meta_gl()
{
    metagl::Initialize(fake_loader);

    int release_count = 0;
    int recreate_count = 0;

    struct FakeResource : easygl::RecoverableResource
    {
        int& released;
        int& recreated;
        FakeResource(int& rel, int& rec) : released(rel), recreated(rec) {}
        void release_gl_handle_only() override { ++released; }
        void recreate_gl_resource()   override { ++recreated; }
    };

    FakeResource res(release_count, recreate_count);

    easygl::ResourceRegistry registry;
    registry.add(&res);
    registry.register_with_meta_gl();

    metagl::NotifyContextLost();
    assert(release_count == 1);

    metagl::Initialize(fake_loader);
    metagl::NotifyContextRestored();
    assert(recreate_count == 1);

    registry.unregister_from_meta_gl();

    // Should not trigger again.
    metagl::NotifyContextLost();
    assert(release_count == 1);
    metagl::Initialize(fake_loader);

    std::cout << "  [PASS] test_resource_registry_wired_to_meta_gl\n";
}

// ---------------------------------------------------------------------------
// Test: stale generation makes a resource invalid
// ---------------------------------------------------------------------------
static void test_stale_generation()
{
    // Initialize context once → generation = N.
    metagl::Initialize(fake_loader);
    const std::uint64_t gen1 = metagl::GetContextGeneration();

    // Query generation from meta-gl directly; simulate a resource created now.
    assert(metagl::GetContextGeneration() == gen1);

    // Simulate a context restore which increments the generation.
    metagl::Initialize(fake_loader);
    const std::uint64_t gen2 = metagl::GetContextGeneration();
    assert(gen2 == gen1 + 1u);

    // A resource recorded with gen1 is now stale (gen1 != gen2).
    assert(gen1 != gen2 && "Generations must differ after restore");

    std::cout << "  [PASS] test_stale_generation (gen1=" << gen1 << " gen2=" << gen2 << ")\n";
}

// ---------------------------------------------------------------------------
// Test: GLES 2.0-only loader initializes successfully (no GLES 3+ functions)
// ---------------------------------------------------------------------------
static void test_gles2_only_loader_succeeds()
{
    bool ok = metagl::Initialize(gles2_only_loader);
    assert(ok && "Initialize must succeed with a GLES 2.0-only loader");

    const metagl::Capabilities& caps = metagl::GetCapabilities();
    assert(caps.gles20 && "GLES 2.0 flag must be set");
    assert(!caps.gles30 && "GLES 3.0 flag must NOT be set for a GLES 2.0-only context");
    assert(!caps.gles31 && "GLES 3.1 flag must NOT be set");
    assert(!caps.gles32 && "GLES 3.2 flag must NOT be set");

    // All 10 previously-missing GLES 2.0 functions must now be available.
    assert(metagl::IsFunctionAvailable("glBindAttribLocation") &&
           "glBindAttribLocation must be available in GLES 2.0 context");
    assert(metagl::IsFunctionAvailable("glBlendColor") &&
           "glBlendColor must be available in GLES 2.0 context");
    assert(metagl::IsFunctionAvailable("glBlendEquationSeparate") &&
           "glBlendEquationSeparate must be available in GLES 2.0 context");
    assert(metagl::IsFunctionAvailable("glBlendFuncSeparate") &&
           "glBlendFuncSeparate must be available in GLES 2.0 context");
    assert(metagl::IsFunctionAvailable("glGetAttachedShaders") &&
           "glGetAttachedShaders must be available in GLES 2.0 context");
    assert(metagl::IsFunctionAvailable("glGetShaderPrecisionFormat") &&
           "glGetShaderPrecisionFormat must be available in GLES 2.0 context");
    assert(metagl::IsFunctionAvailable("glGetShaderSource") &&
           "glGetShaderSource must be available in GLES 2.0 context");
    assert(metagl::IsFunctionAvailable("glIsEnabled") &&
           "glIsEnabled must be available in GLES 2.0 context");
    assert(metagl::IsFunctionAvailable("glReleaseShaderCompiler") &&
           "glReleaseShaderCompiler must be available in GLES 2.0 context");
    assert(metagl::IsFunctionAvailable("glShaderBinary") &&
           "glShaderBinary must be available in GLES 2.0 context");

    // GLES 3+ functions must be reported as unavailable.
    assert(!metagl::IsFunctionAvailable("glGenVertexArrays") &&
           "glGenVertexArrays must be unavailable in GLES 2.0 context");
    assert(!metagl::IsFunctionAvailable("glDispatchCompute") &&
           "glDispatchCompute must be unavailable in GLES 2.0 context");

    std::cout << "  [PASS] test_gles2_only_loader_succeeds\n";
}

// ---------------------------------------------------------------------------
// Test: GLES 3.0 loader (no GLES 3.1/3.2) initializes successfully
// ---------------------------------------------------------------------------
static void test_gles30_loader_succeeds()
{
    bool ok = metagl::Initialize(gles3_only_loader);
    assert(ok && "Initialize must succeed with a GLES 3.0 loader (no 3.1/3.2)");

    const metagl::Capabilities& caps = metagl::GetCapabilities();
    assert(caps.gles20 && "GLES 2.0 flag must be set");
    assert(caps.gles30 && "GLES 3.0 flag must be set");
    assert(!caps.gles31 && "GLES 3.1 flag must NOT be set");
    assert(!caps.gles32 && "GLES 3.2 flag must NOT be set");

    // GLES 3.1+ functions must be reported as unavailable.
    assert(!metagl::IsFunctionAvailable("glDispatchCompute") &&
           "glDispatchCompute must be unavailable in GLES 3.0 context");
    assert(!metagl::IsFunctionAvailable("glPatchParameteri") &&
           "glPatchParameteri must be unavailable in GLES 3.0 context");

    // GLES 3.0 functions should be available.
    assert(metagl::IsFunctionAvailable("glGenVertexArrays") &&
           "glGenVertexArrays must be available in GLES 3.0 context");

    std::cout << "  [PASS] test_gles30_loader_succeeds\n";
}

// ---------------------------------------------------------------------------
// Test: loader missing a required GLES 2.0 function fails initialization
// ---------------------------------------------------------------------------
static void test_missing_required_gles2_function_fails()
{
    bool ok = metagl::Initialize(gles2_missing_required_loader);
    assert(!ok && "Initialize must fail when a required GLES 2.0 function is missing");

    // IsFunctionAvailable must correctly report missing GLES 2.0 functions.
    assert(!metagl::IsFunctionAvailable("glDrawArrays") &&
           "glDrawArrays must be reported as unavailable");
    assert(!metagl::IsFunctionAvailable("glBindAttribLocation") &&
           "glBindAttribLocation must be reported as unavailable");

    // Restore a good context for subsequent tests.
    metagl::Initialize(fake_loader);

    std::cout << "  [PASS] test_missing_required_gles2_function_fails\n";
}

// ---------------------------------------------------------------------------
// Test: IsFunctionAvailable returns false for GLES3+ funcs in GLES2 context
// ---------------------------------------------------------------------------
static void test_function_availability_tiered()
{
    // Initialize with GLES 2.0-only loader.
    bool ok = metagl::Initialize(gles2_only_loader);
    assert(ok);

    // GLES 2.0 core function — must be true.
    assert(metagl::IsFunctionAvailable("glDrawArrays"));
    assert(metagl::IsFunctionAvailable("glCreateShader"));
    assert(metagl::IsFunctionAvailable("glBindTexture"));

    // GLES 3.0+ function — must be false.
    assert(!metagl::IsFunctionAvailable("glGenVertexArrays"));
    assert(!metagl::IsFunctionAvailable("glBindBufferBase"));
    assert(!metagl::IsFunctionAvailable("glFenceSync"));

    // Now re-initialize with full GLES 3.0 loader.
    ok = metagl::Initialize(gles3_only_loader);
    assert(ok);

    // GLES 3.0 function — must now be true.
    assert(metagl::IsFunctionAvailable("glGenVertexArrays"));
    assert(metagl::IsFunctionAvailable("glFenceSync"));

    // GLES 3.1+ function — still false.
    assert(!metagl::IsFunctionAvailable("glDispatchCompute"));

    // Restore for subsequent tests.
    metagl::Initialize(fake_loader);

    std::cout << "  [PASS] test_function_availability_tiered\n";
}

// ---------------------------------------------------------------------------
// main
// ---------------------------------------------------------------------------
int main()
{
    try
    {
        std::cout << "Running context lifecycle tests…\n";
        test_generation_increments();
        test_context_lost_status();
        test_is_function_available();
        test_capabilities_from_mocked_strings();
        test_context_events();
        test_resource_registry_context_lost();
        test_resource_registry_wired_to_meta_gl();
        test_stale_generation();
        test_gles2_only_loader_succeeds();
        test_gles30_loader_succeeds();
        test_missing_required_gles2_function_fails();
        test_function_availability_tiered();
        std::cout << "All context lifecycle tests passed.\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Test failed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
