/// Tests for the following WebGL-related correctness fixes (originally
/// tracked in the now-removed webgl.md, then TODO.md; all fully implemented):
///  - Device::initialize() reuses meta-gl's own context/version detection
///    instead of duplicating GL_VERSION-string parsing.
///  - Query/Sampler/TransformFeedback/Sync/ProgramPipeline, and
///    Texture::get_level_parameter(f)(), throw a clean
///    UnsupportedFeatureException instead of crashing on a null function
///    pointer when the underlying entry point is missing (as on WebGL 1).
///  - VertexArrayObject support on a WebGL-1-shaped context depends on the
///    optional GL_OES_vertex_array_object extension, not just the version.
///
/// No real GPU/browser context is required; a fake function-pointer loader
/// simulating a WebGL 1-shaped context (GLES 2.0 core only, no GLES 3.0+
/// entry points) is used. When this file is cross-compiled with Emscripten,
/// the same loader additionally exercises meta-gl's real
/// ApiKind::WebGL/webgl1 detection, which is only compiled in under
/// __EMSCRIPTEN__.

#include <easygl/easygl.hpp>
#include <metagl/metagl.hpp>

#include <cassert>
#include <iostream>
#include <string>

namespace
{
    /// Set of GLES 3.0+ function names that a WebGL 1 context does not provide.
    /// Intentionally the same representative sample already used by
    /// ContextLifecycleTests.cpp's gles2_only_loader.
    bool is_gles3_only(const std::string& n)
    {
        static const char* gles3_fns[] = {
            "glGenVertexArrays", "glDeleteVertexArrays", "glBindVertexArray", "glIsVertexArray",
            "glGetStringi", "glGenSamplers", "glDeleteSamplers", "glBindSampler",
            "glGenTransformFeedbacks", "glDeleteTransformFeedbacks", "glBindTransformFeedback",
            "glFenceSync", "glDeleteSync", "glIsSync", "glClientWaitSync", "glWaitSync",
            "glGenQueries", "glDeleteQueries", "glBeginQuery", "glEndQuery",
            "glGetTexLevelParameteriv", "glGetTexLevelParameterfv",
            // GLES 3.1+
            "glGenProgramPipelines", "glDeleteProgramPipelines", "glBindProgramPipeline",
            nullptr
        };
        for (int i = 0; gles3_fns[i] != nullptr; ++i)
            if (n == gles3_fns[i]) return true;
        return false;
    }

    /// Simulates a WebGL 1 context: GLES 2.0 core entry points only, GL_VERSION
    /// reported the way real browsers report it via Emscripten
    /// ("OpenGL ES 2.0 (WebGL 1.0 ...)"). GL_OES_vertex_array_object is
    /// advertised, matching what real browsers overwhelmingly support in
    /// practice, so that Device::initialize() (which requires
    /// Feature::VertexArrayObject unconditionally) succeeds and the rest of
    /// the gating below the ES 3.0 threshold can be exercised.
    void* webgl1_loader(const char* name)
    {
        const std::string n(name);

        if (is_gles3_only(n))
            return nullptr;

        if (n == "glGetString")
        {
            return reinterpret_cast<void*>(+[](unsigned int pname) -> const unsigned char*
            {
                if (pname == 0x1F02) return reinterpret_cast<const unsigned char*>("OpenGL ES 2.0 (WebGL 1.0 (OpenGL ES 2.0 Chromium))");
                // GL_EXTENSIONS (old-style, space-separated): used for major < 3.
                if (pname == 0x1F03) return reinterpret_cast<const unsigned char*>("GL_OES_vertex_array_object");
                return reinterpret_cast<const unsigned char*>("");
            });
        }
        if (n == "glGetIntegerv")
        {
            return reinterpret_cast<void*>(+[](unsigned int pname, int* data)
            {
                if (pname == 0x821B) *data = 2;   // GL_MAJOR_VERSION
                if (pname == 0x821C) *data = 0;   // GL_MINOR_VERSION
                if (pname == 0x821D) *data = 0;   // GL_NUM_EXTENSIONS
                if (pname == 0x0D33) *data = 1024; // GL_MAX_TEXTURE_SIZE
            });
        }
        // Non-null sentinel for every other GLES 2.0 core function.
        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xDEADBEEF));
    }

    /// Same as webgl1_loader, but WITHOUT GL_OES_vertex_array_object — the
    /// worst (spec-allowed) case for bod 5 (VertexArrayObject was previously
    /// required unconditionally). Used to verify Device::initialize() fails
    /// with a clear exception rather than silently misbehaving.
    void* webgl1_no_vao_loader(const char* name)
    {
        const std::string n(name);

        if (is_gles3_only(n))
            return nullptr;

        if (n == "glGetString")
        {
            return reinterpret_cast<void*>(+[](unsigned int pname) -> const unsigned char*
            {
                if (pname == 0x1F02) return reinterpret_cast<const unsigned char*>("OpenGL ES 2.0 (WebGL 1.0 (OpenGL ES 2.0 Chromium))");
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
                if (pname == 0x0D33) *data = 1024;
            });
        }
        return reinterpret_cast<void*>(static_cast<std::uintptr_t>(0xDEADBEEF));
    }
}

static void test_webgl1_context_detected_and_gated()
{
    easygl::Device device;
    device.initialize(webgl1_loader);

    const auto& caps = device.capabilities();

#ifdef __EMSCRIPTEN__
    // Only compiled in by meta-gl's Context.cpp under Emscripten.
    assert(caps.is_webgl() && "A WebGL 1-shaped GL_VERSION string must be classified as ApiKind::WebGL under Emscripten");
    assert(caps.is_webgl1() && "webgl1 flag must be set for a WebGL 1 context");
    assert(!caps.is_webgl2() && "webgl2 flag must NOT be set for a WebGL 1 context");
#else
    // Native builds fall back to plain OpenGLES classification; the point of
    // this assertion is that ApiKind::WebGL must never leak into a native build.
    assert(caps.is_opengles());
    assert(!caps.is_webgl());
#endif

    // Bod 5: below GLES/WebGL 3.0, VertexArrayObject support must depend on
    // GL_OES_vertex_array_object rather than being assumed unconditionally;
    // webgl1_loader advertises it, so it must be reported as supported here.
    assert(caps.supports(easygl::Feature::VertexArrayObject) &&
           "VertexArrayObject must be supported when GL_OES_vertex_array_object is advertised");

    std::cout << "  [PASS] test_webgl1_context_detected_and_gated\n";
}

static void test_webgl1_without_vao_extension_fails_clearly()
{
    // Bod 5: Device::initialize() requires Feature::VertexArrayObject
    // unconditionally. On a WebGL-1-shaped context that does NOT advertise
    // the optional GL_OES_vertex_array_object extension, this must now fail
    // with a clear, explicit Exception (not a null-pointer crash somewhere
    // deep inside VAO-using code later on).
    easygl::Device device;

    bool threw = false;
    try
    {
        device.initialize(webgl1_no_vao_loader);
    }
    catch (const easygl::Exception&)
    {
        threw = true;
    }
    assert(threw && "Device::initialize() must throw a clear Exception when GL_OES_vertex_array_object is missing on a sub-3.0 WebGL context");

    std::cout << "  [PASS] test_webgl1_without_vao_extension_fails_clearly\n";
}

static void test_query_throws_instead_of_crashing()
{
    easygl::Device device;
    device.initialize(webgl1_loader);

    bool threw = false;
    try
    {
        easygl::Query q;
        q.create();
    }
    catch (const easygl::UnsupportedFeatureException&)
    {
        threw = true;
    }
    assert(threw && "Query::create() must throw UnsupportedFeatureException, not crash, when glGenQueries is unavailable");

    std::cout << "  [PASS] test_query_throws_instead_of_crashing\n";
}

static void test_sampler_throws_instead_of_crashing()
{
    easygl::Device device;
    device.initialize(webgl1_loader);

    bool threw = false;
    try
    {
        easygl::Sampler s;
        s.create();
    }
    catch (const easygl::UnsupportedFeatureException&)
    {
        threw = true;
    }
    assert(threw && "Sampler::create() must throw UnsupportedFeatureException, not crash, when glGenSamplers is unavailable");

    std::cout << "  [PASS] test_sampler_throws_instead_of_crashing\n";
}

static void test_transform_feedback_throws_instead_of_crashing()
{
    easygl::Device device;
    device.initialize(webgl1_loader);

    bool threw = false;
    try
    {
        easygl::TransformFeedback tf;
        tf.create();
    }
    catch (const easygl::UnsupportedFeatureException&)
    {
        threw = true;
    }
    assert(threw && "TransformFeedback::create() must throw UnsupportedFeatureException, not crash, when glGenTransformFeedbacks is unavailable");

    std::cout << "  [PASS] test_transform_feedback_throws_instead_of_crashing\n";
}

static void test_sync_throws_instead_of_crashing()
{
    easygl::Device device;
    device.initialize(webgl1_loader);

    bool threw = false;
    try
    {
        easygl::Sync sync;
        sync.fence(easygl::SyncCondition::GpuCommandsComplete);
    }
    catch (const easygl::UnsupportedFeatureException&)
    {
        threw = true;
    }
    assert(threw && "Sync::fence() must throw UnsupportedFeatureException, not crash, when glFenceSync is unavailable");

    std::cout << "  [PASS] test_sync_throws_instead_of_crashing\n";
}

static void test_program_pipeline_throws_instead_of_crashing()
{
    easygl::Device device;
    device.initialize(webgl1_loader);

    bool threw = false;
    try
    {
        easygl::ProgramPipeline pipeline;
        pipeline.create();
    }
    catch (const easygl::UnsupportedFeatureException&)
    {
        threw = true;
    }
    assert(threw && "ProgramPipeline::create() must throw UnsupportedFeatureException, not crash — ProgramPipeline has no WebGL equivalent at all");

    std::cout << "  [PASS] test_program_pipeline_throws_instead_of_crashing\n";
}

static void test_texture_level_parameter_throws_instead_of_crashing()
{
    easygl::Device device;
    device.initialize(webgl1_loader);

    // The availability check happens before any real GL call is made, so an
    // uncreated (handle_ == 0) Texture is sufficient to exercise the guard —
    // no need to actually allocate a texture through the many other (here
    // unstubbed) GL entry points that create_2d() would otherwise call.
    easygl::Texture tex;

    bool threw = false;
    try
    {
        (void)tex.get_level_parameter(easygl::TextureTarget::Texture2D, 0, easygl::TextureLevelParameter::Width);
    }
    catch (const easygl::UnsupportedFeatureException&)
    {
        threw = true;
    }
    assert(threw && "Texture::get_level_parameter() must throw UnsupportedFeatureException, not crash, when glGetTexLevelParameteriv is unavailable");

    std::cout << "  [PASS] test_texture_level_parameter_throws_instead_of_crashing\n";
}

int main()
{
    try
    {
        std::cout << "Running WebGL tests...\n";
        test_webgl1_context_detected_and_gated();
        test_webgl1_without_vao_extension_fails_clearly();
        test_query_throws_instead_of_crashing();
        test_sampler_throws_instead_of_crashing();
        test_transform_feedback_throws_instead_of_crashing();
        test_sync_throws_instead_of_crashing();
        test_program_pipeline_throws_instead_of_crashing();
        test_texture_level_parameter_throws_instead_of_crashing();
        std::cout << "All WebGL tests passed.\n";
    }
    catch (const std::exception& e)
    {
        std::cerr << "Test failed: " << e.what() << "\n";
        return 1;
    }
    return 0;
}
