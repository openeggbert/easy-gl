# easy-gl Improvement Plan

This document lists concrete tasks to improve easy-gl's correctness, completeness,
and usability.  Tasks are grouped by theme and ordered roughly by priority within
each group.

---

## Group A — API correctness: remove raw handles from the public interface

### A1 — `Framebuffer::attach_texture_2d` should accept `const Texture&`

**Current:**
```cpp
void attach_texture_2d(FramebufferTarget, FramebufferAttachment,
                       TextureTarget, unsigned int texture, int level);
```
**Target:**
```cpp
void attach_texture_2d(FramebufferTarget, FramebufferAttachment,
                       TextureTarget, const Texture& texture, int level);
```
Raw `unsigned int` handle forces callers to call `native_handle()` manually and
breaks type safety.  Repeat for `attach_texture_layer`.

---

### A2 — `Framebuffer::attach_renderbuffer` should accept `const Renderbuffer&`

**Current:**
```cpp
void attach_renderbuffer(FramebufferTarget, FramebufferAttachment,
                         unsigned int renderbuffer);
```
**Target:**
```cpp
void attach_renderbuffer(FramebufferTarget, FramebufferAttachment,
                         const Renderbuffer& renderbuffer);
```

---

### A3 — `ProgramPipeline::use_stages` and `set_active_shader_program` should accept `const Program&`

**Current:**
```cpp
void use_stages(ShaderStageMask stages, unsigned int program);
void set_active_shader_program(unsigned int program);
```
**Target:**
```cpp
void use_stages(ShaderStageMask stages, const Program& program);
void set_active_shader_program(const Program& program);
```

---

### A4 — `TransformFeedback::set_varyings` should accept `const Program&`

**Current:**
```cpp
static void set_varyings(unsigned int program, ...);
```
**Target:**
```cpp
static void set_varyings(const Program& program, ...);
```

---

### A5 — Replace `GL_INVALID_INDEX` constant with a typed sentinel in `Program`

`Program::uniform_block_index` returns `GL_INVALID_INDEX` (raw GL constant) when
the block is not found.  Either return `std::optional<unsigned int>` or define an
`easygl::invalid_index` constant so users never need to include any GL header.

---

### A6 — Fix undocumented side-effects in `Texture::set_image_2d` convenience overload

The overload `set_image_2d(target, level, width, height, data)` silently:
- binds the texture
- sets pixel unpack alignment to 1
- sets MinFilter, MagFilter, WrapS, WrapT

Document these side-effects clearly in the header, or split into an explicit
`upload_rgba8` named factory that lists what it does.

---

### A7 — `Device::clear` silently disables `ScissorTest`

`Device::clear()` calls `glDisable(ScissorTest)` before clearing, which mutates
render state the caller may have set intentionally.  Either document this strongly,
or remove the implicit `glDisable` and let the caller manage scissor state.

---

## Group B — New `std::span`-based typed upload helpers

### B1 — `Buffer::set_data` typed template overload

Add a template helper so callers pass typed data without casting to `void*`:

```cpp
template<typename T>
void set_data(std::span<const T> data, BufferUsage usage = BufferUsage::StaticDraw);

template<typename T>
void set_data(BufferTarget target, std::span<const T> data,
              BufferUsage usage = BufferUsage::StaticDraw);
```

---

### B2 — `Buffer::set_sub_data` typed template overload

Same pattern as B1 for partial updates:

```cpp
template<typename T>
void set_sub_data(std::span<const T> data, std::size_t offset_in_bytes = 0);
```

---

### B3 — `Texture::set_image_2d` span overload

```cpp
template<typename T>
void set_image_2d(TextureTarget, int level,
                  InternalFormat, int width, int height,
                  PixelFormat, PixelType, std::span<const T> data);
```
Ensures the span size is consistent with `width * height` in debug builds.

---

## Group C — Named constructors / factory methods

### C1 — `Buffer` named factories

```cpp
static Buffer create_vertex_buffer(std::span<const std::byte> data,
                                   BufferUsage = BufferUsage::StaticDraw);
static Buffer create_index_buffer(std::span<const std::byte> data,
                                  BufferUsage = BufferUsage::StaticDraw);
static Buffer create_uniform_buffer(std::size_t size_in_bytes,
                                    BufferUsage = BufferUsage::DynamicDraw);
```

---

### C2 — `Texture` named factories

```cpp
static Texture create_2d(int width, int height,
                         InternalFormat = InternalFormat::Rgba8,
                         bool generate_mipmaps = false);
static Texture create_2d_rgba8(int width, int height,
                                const void* pixels, bool generate_mipmaps = false);
```

---

### C3 — `Framebuffer` named factory for render-to-texture

```cpp
static Framebuffer create_color_depth(const Texture& color,
                                      const Renderbuffer& depth);
```

---

### C4 — `Program` named constructors beyond `compile_from_sources`

```cpp
static Program from_sources(std::string_view vertex, std::string_view fragment);
static Program from_sources(std::string_view vertex,
                            std::string_view geometry,
                            std::string_view fragment);
```

---

## Group D — RAII binding guards

### D1 — `ScopedBind<T>` helper

A generic scoped binder that binds on construction and restores/unbinds on
destruction:

```cpp
// include/easygl/detail/ScopedBind.hpp
template<typename Resource>
class ScopedBind
{
public:
    explicit ScopedBind(const Resource& r, /* bind args */);
    ~ScopedBind();
    ScopedBind(const ScopedBind&) = delete;
    ScopedBind& operator=(const ScopedBind&) = delete;
};
```

Provide specialisations for at least `Texture`, `Buffer`, `VertexArray`,
`Framebuffer`, `Renderbuffer`, `Sampler`.

---

## Group E — Shared generation-tracking base

### E1 — Extract `GenerationTracked` mixin

Every resource class duplicates the same `handle_` + `generation_` + three
identical method bodies (`is_valid_for_current_generation`, `creation_generation`,
`native_handle`).  Extract to:

```cpp
// include/easygl/detail/GenerationTracked.hpp
class GenerationTracked
{
protected:
    unsigned int handle_ = 0;
    std::uint64_t generation_ = 0;
public:
    [[nodiscard]] bool is_valid_for_current_generation() const noexcept;
    [[nodiscard]] std::uint64_t creation_generation() const noexcept;
};
```

Then each resource class inherits `detail::NonCopyable, detail::GenerationTracked`
and removes the duplicated bodies.  `Sync` keeps its `__GLsync*` pointer
separately.

---

## Group F — State query methods (`glGet*`)

### F1 — Generic `Device` scalar getters

Add to `Device`:

```cpp
[[nodiscard]] bool       get_boolean(GetParameter pname) const;
[[nodiscard]] float      get_float(GetParameter pname) const;
[[nodiscard]] int        get_integer(GetParameter pname) const;
[[nodiscard]] int64_t    get_integer64(GetParameter pname) const;
[[nodiscard]] int        get_integeri(GetParameter pname, unsigned int index) const;
[[nodiscard]] int64_t    get_integer64i(GetParameter pname, unsigned int index) const;
[[nodiscard]] bool       get_booleani(GetParameter pname, unsigned int index) const;
```

---

### F2 — `Texture` parameter getters

```cpp
void get_parameter_fv(TextureTarget, TextureParameter, float* out) const;
void get_parameter_iv(TextureTarget, TextureParameter, int* out) const;
[[nodiscard]] int   get_level_parameter(TextureTarget, int level, TextureLevelParameter) const;
[[nodiscard]] float get_level_parameterf(TextureTarget, int level, TextureLevelParameter) const;
```

---

### F3 — `Buffer` parameter getters

```cpp
[[nodiscard]] int     get_parameter(BufferTarget, BufferParameter) const;
[[nodiscard]] int64_t get_parameter64(BufferTarget, BufferParameter) const;
[[nodiscard]] void*   get_pointer(BufferTarget) const;
```

---

### F4 — `VertexArray` attribute getters

```cpp
void get_attribute_fv(unsigned int index, VertexAttribParameter, float* out) const;
void get_attribute_iv(unsigned int index, VertexAttribParameter, int* out) const;
[[nodiscard]] void* get_attribute_pointer(unsigned int index) const;
```

---

### F5 — `Program` uniform value getters

```cpp
void get_uniform_fv(int location, float* out) const;
void get_uniform_iv(int location, int* out) const;
void get_uniform_uiv(int location, unsigned int* out) const;
```

---

### F6 — `Program` introspection methods

```cpp
// active attributes / uniforms
[[nodiscard]] int active_attrib_count() const;
[[nodiscard]] int active_uniform_count() const;
// active uniform block info
[[nodiscard]] std::string uniform_block_name(unsigned int block_index) const;
// fragment data location
[[nodiscard]] int frag_data_location(const std::string& name) const;
```

---

### F7 — `Framebuffer` parameter getters

```cpp
[[nodiscard]] int get_attachment_parameter(FramebufferTarget, FramebufferAttachment,
                                            FramebufferAttachmentParameter) const;
```

---

### F8 — `Renderbuffer` parameter getter

```cpp
[[nodiscard]] int get_parameter(RenderbufferParameter pname) const;
```

---

### F9 — `Sampler` parameter getters

```cpp
void get_parameter_fv(TextureParameter, float* out) const;
void get_parameter_iv(TextureParameter, int* out) const;
```

---

### F10 — `Query` parameter getter

```cpp
[[nodiscard]] unsigned int result_u64() const;   // 64-bit timer query result
```

---

### F11 — `Sync` parameter getter

```cpp
[[nodiscard]] int get_parameter(SyncParameter pname) const;
```

---

## Group G — GL object existence checks (`glIs*`)

### G1 — `is_valid_gl_object()` on each resource class

Add a method that calls the GL validator (not just checking `handle_ != 0`):

```cpp
// Buffer
[[nodiscard]] bool is_valid_gl_object() const;  // calls glIsBuffer
// Texture      → glIsTexture
// Program      → glIsProgram
// Shader       → glIsShader
// VertexArray  → glIsVertexArray
// Framebuffer  → glIsFramebuffer
// Renderbuffer → glIsRenderbuffer
// Sampler      → glIsSampler
// Query        → glIsQuery
// Sync         → glIsSync
// TransformFeedback → glIsTransformFeedback
// ProgramPipeline   → glIsProgramPipeline
```

---

### G2 — `Device::is_enabled(Capability)` and `Device::is_enabled(Capability, unsigned int index)`

```cpp
[[nodiscard]] bool is_enabled(Capability) const;
[[nodiscard]] bool is_enabled(Capability, unsigned int index) const;
```

---

## Group H — Missing uniform helpers

### H1 — `Program::set_uniform_iv` and `set_uniform_uiv` array variants

```cpp
void set_uniform_iv(int location, std::span<const int> values, int components = 1);
void set_uniform_uiv(int location, std::span<const unsigned int> values, int components = 1);
```

---

### H2 — `Program::set_program_uniform*` variants for separable programs

Mirror the full `set_uniform` and matrix family as `set_program_uniform` static or
instance methods so `ProgramPipeline` users can set uniforms without binding the
program:

```cpp
// float
void set_program_uniform(unsigned int program, int location, float);
void set_program_uniform(unsigned int program, int location, float, float);
// ... int, uint, matrix variants ...
```
(Fully implemented; see `Program.hpp`/`Program.cpp` for the complete list of
`set_program_uniform*` overloads.)

---

## Group I — Advanced draw calls

### I1 — Add missing draw primitives to `Device`

```cpp
void draw_range_elements(PrimitiveType, unsigned int start, unsigned int end,
                          int count, DataType, const void* indices);
void draw_elements_base_vertex(PrimitiveType, int count, DataType,
                                const void* indices, int base_vertex);
void draw_arrays_indirect(PrimitiveType, const void* indirect);
void draw_elements_indirect(PrimitiveType, DataType, const void* indirect);
void dispatch_compute_indirect(std::ptrdiff_t indirect);
```

---

## Group J — Texture completeness

### J1 — Copy operations

```cpp
void copy_image_2d(TextureTarget, int level, InternalFormat,
                   int x, int y, int width, int height);
void copy_sub_image_2d(TextureTarget, int level,
                        int xoff, int yoff, int x, int y, int width, int height);
static void copy_image_sub_data(const Texture& src, TextureTarget src_target,
                                 int src_level, int sx, int sy, int sz,
                                 const Texture& dst, TextureTarget dst_target,
                                 int dst_level, int dx, int dy, int dz,
                                 int width, int height, int depth);
```

---

### J2 — Buffer textures

```cpp
void set_buffer(TextureTarget, InternalFormat, const Buffer& buffer);
void set_buffer_range(TextureTarget, InternalFormat, const Buffer& buffer,
                       std::ptrdiff_t offset, std::ptrdiff_t size);
```

---

### J3 — Compressed sub-image update

```cpp
void set_compressed_sub_image_2d(TextureTarget, int level,
                                   int x, int y, int width, int height,
                                   CompressedInternalFormat, std::size_t image_size,
                                   const void* data);
```

---

### J4 — Array/vector `set_parameter` overloads on `Texture`

```cpp
void set_parameter_fv(TextureTarget, TextureParameter, const float* values);
void set_parameter_iv(TextureTarget, TextureParameter, const int* values);
void set_parameter_iiv(TextureTarget, TextureParameter, const int* values);
void set_parameter_iuiv(TextureTarget, TextureParameter, const unsigned int* values);
```

---

## Group K — Vertex format separation (ES 3.1+)

### K1 — Separate vertex format from buffer binding in `VertexArray`

```cpp
void set_attribute_format(unsigned int attrib_index, int size,
                           DataType type, bool normalized,
                           unsigned int relative_offset);
void set_attribute_i_format(unsigned int attrib_index, int size,
                             DataType type, unsigned int relative_offset);
void set_attribute_binding(unsigned int attrib_index, unsigned int binding_index);
void bind_vertex_buffer(unsigned int binding_index, const Buffer& buffer,
                         std::ptrdiff_t offset, std::size_t stride);
void set_binding_divisor(unsigned int binding_index, unsigned int divisor);
```

---

## Group L — Constant vertex attribute values

### L1 — `VertexArray::set_attrib_constant` overloads

```cpp
void set_attrib_constant(unsigned int index, float x);
void set_attrib_constant(unsigned int index, float x, float y);
void set_attrib_constant(unsigned int index, float x, float y, float z);
void set_attrib_constant(unsigned int index, float x, float y, float z, float w);
void set_attrib_constant(unsigned int index, int x, int y, int z, int w);
void set_attrib_constant(unsigned int index, unsigned int x, unsigned int y,
                          unsigned int z, unsigned int w);
```

---

## Group M — Debug message API

### M1 — `Device` debug callback and message control

```cpp
using DebugCallback = void(*)(DebugSource, DebugType, unsigned int id,
                               DebugSeverity, std::string_view message);
void set_debug_callback(DebugCallback callback, void* user_param = nullptr);
void set_debug_message_control(DebugSource, DebugType, DebugSeverity,
                                std::span<const unsigned int> ids, bool enabled);
void insert_debug_message(DebugSource, DebugType, unsigned int id,
                           DebugSeverity, std::string_view message);
[[nodiscard]] std::string get_object_label(DebugObjectLabel, unsigned int name) const;
```

---

## Group N — Framebuffer extras

### N1 — Layered texture attachment

```cpp
void attach_texture(FramebufferTarget, FramebufferAttachment,
                    const Texture& texture, int level);
```

---

### N2 — Attachmentless framebuffer parameter

```cpp
void set_parameter(FramebufferTarget, FramebufferDefaultParameter, int value);
```

---

### N3 — Sub-framebuffer invalidation

```cpp
void invalidate_sub(FramebufferTarget, std::span<const FramebufferAttachment>,
                    int x, int y, int width, int height);
```

---

## Group O — Program and shader binary

### O1 — `Program::load_binary`

```cpp
void load_binary(ProgramBinaryFormat format, const void* data, std::size_t length);
```

---

### O2 — `Program::set_parameter` and separable programs

```cpp
void set_parameter(ProgramParameter pname, int value);
static Program create_separable(ShaderType, std::string_view source);
```

---

### O3 — `Shader::load_binary`

```cpp
void load_binary(ShaderBinaryFormat format, const void* data, std::size_t length);
```

---

### O4 — `Device::release_shader_compiler`

```cpp
void release_shader_compiler();
```

---

## Group P — ES 3.1+/3.2+ miscellaneous

### P1 — `Device::set_sample_mask`

```cpp
void set_sample_mask(unsigned int mask_number, unsigned int mask);
```

---

### P2 — `Device::set_min_sample_shading`

```cpp
void set_min_sample_shading(float value);
```

---

### P3 — `Device::blend_barrier`

```cpp
void blend_barrier();
```

---

### P4 — `Device::set_primitive_bounding_box` (ES 3.2+)

```cpp
void set_primitive_bounding_box(float min_x, float min_y, float min_z, float min_w,
                                 float max_x, float max_y, float max_z, float max_w);
```

---

### P5 — `Device::read_pixels_robust`

```cpp
void read_pixels_robust(int x, int y, int width, int height,
                         PixelFormat, PixelType, std::size_t buf_size, void* pixels);
```

---

### P6 — Per-draw-buffer indexed blend and color state in `Device`

```cpp
void set_blend_func(unsigned int index, BlendFactor src, BlendFactor dst);
void set_blend_func_separate(unsigned int index, BlendFactor src_rgb, BlendFactor dst_rgb,
                              BlendFactor src_alpha, BlendFactor dst_alpha);
void set_blend_equation(unsigned int index, BlendEquation mode);
void set_blend_equation_separate(unsigned int index, BlendEquation rgb, BlendEquation alpha);
void set_color_mask(unsigned int index, bool r, bool g, bool b, bool a);
void enable(Capability, unsigned int index);
void disable(Capability, unsigned int index);
```

---

## Group Q — Higher-level convenience helpers (new files)

### Q1 — `UniformCache` — cache uniform locations by name

```cpp
// include/easygl/UniformCache.hpp
class EASYGL_API UniformCache
{
public:
    explicit UniformCache(const Program& program);
    [[nodiscard]] int operator[](std::string_view name);
private:
    const Program* program_;
    std::unordered_map<std::string, int> cache_;
};
```
Avoids repeated `glGetUniformLocation` calls for the same name.

---

### Q2 — `ScopedDebugGroup` — RAII push/pop debug group

```cpp
// include/easygl/ScopedDebugGroup.hpp
class EASYGL_API ScopedDebugGroup
{
public:
    ScopedDebugGroup(Device& device, std::string_view label, unsigned int id = 0);
    ~ScopedDebugGroup();
    ScopedDebugGroup(const ScopedDebugGroup&) = delete;
    ScopedDebugGroup& operator=(const ScopedDebugGroup&) = delete;
private:
    Device* device_;
};
```

---

### Q3 — `Sampler` preset factory methods

```cpp
static Sampler create_linear_clamp();
static Sampler create_nearest_clamp();
static Sampler create_linear_repeat();
static Sampler create_mipmap_linear();
```

---

## Group R — `Sync` portability fix

### R1 — Replace `struct __GLsync` forward declaration with opaque typedef

`struct __GLsync` is a GCC/glibc implementation detail.  Use the standard `GLsync`
typedef (which is `void*` on most platforms) exposed through meta-gl instead, to
avoid including platform-specific GL internals in the public header.

---

## Group S — `Config` extensions

### S1 — Add configurable debug-log sink to `Config`

```cpp
struct EASYGL_API Config
{
    bool throw_on_missing_feature = true;
    bool enable_debug_logging     = false;
    bool validate_calls           = true;
    // NEW:
    std::function<void(std::string_view)> log_callback;
};
```
Without this, `enable_debug_logging` has nowhere to send its output.

---

### S2 — Add `auto_error_check` flag to `Config`

When `true`, `Device` calls `glGetError()` after every operation (debug builds
only) and invokes `log_callback` on non-`NoError` results.

---

## Group T — `ResourceRegistry` usability

### T1 — RAII registration guard

```cpp
// include/easygl/ResourceRegistration.hpp
class EASYGL_API ResourceRegistration
{
public:
    ResourceRegistration(ResourceRegistry& registry, RecoverableResource& resource);
    ~ResourceRegistration();
    ResourceRegistration(const ResourceRegistration&) = delete;
    ResourceRegistration& operator=(const ResourceRegistration&) = delete;
private:
    ResourceRegistry* registry_;
    RecoverableResource* resource_;
};
```
Removes the need for callers to call `registry.remove()` manually.

---

## Group U — Testing

### U1 — Set up a headless test suite

Add a `tests/` directory with a CMake target `easy-gl-tests`.  Use a software
rasterizer (Mesa / `llvmpipe`, available in CI) or an EGL pbuffer so tests run
without a display.  Target coverage:

- Buffer: create/destroy, typed upload template, `is_valid_gl_object()`
- Texture: create/destroy, set_image_2d, generate_mipmap
- Program: compile_from_sources, uniform_location, set_uniform round-trip
- VertexArray: create/bind/set_attribute
- Framebuffer: create, attach_texture_2d, is_complete
- Device: initialize, clear, draw_arrays

---

### U2 — Test context-loss / ResourceRegistry recovery cycle

Simulate context loss by calling `on_context_lost()` + `on_context_restored()` on
a registry that holds a concrete `RecoverableResource` subclass.  Verify handles
are zeroed after loss and recreated after restore.

---

## Group V — Examples

### V1 — Add a textured-quad example

Extend the `examples/` directory with a `hello-textured-quad` that demonstrates
`Texture`, `Sampler`, and `set_image_2d` usage.

---

### V2 — Add an offscreen rendering example

Demonstrate `Framebuffer` + `Renderbuffer` (depth) + `Texture` (color) +
`Device::read_pixels` to render to texture and save a PNG.

---

### V3 — Add a UBO example

Demonstrate `Buffer` with `BufferTarget::UniformBuffer`, `Program::uniform_block_index`,
`set_uniform_block_binding`, and `bind_base`.

---

## Group W — Build / CMake hygiene

### W1 — Align CMakeLists C++ standard setting

`set_target_properties` sets `CXX_STANDARD 23` while CLAUDE.md says the target
baseline is C++20.  Decide the actual minimum, document it, and align
`CXX_STANDARD` and `target_compile_features` to match.

---

### W2 — Add `EASYGL_BUILD_TESTS` option and test directory skeleton

`CMakeLists.txt` already references `tests/` via `if(EASYGL_BUILD_TESTS)` but the
directory does not exist.  Create it with a minimal `CMakeLists.txt` and one
placeholder test so the option builds cleanly.

---

### W3 — Add `cmake/EasyGlInstall.cmake` for proper package export

Currently `install()` sets up the target but there is no `easy-glConfig.cmake`
package file.  Add `configure_package_config_file` and `write_basic_package_version_file`
so downstream projects can `find_package(easy-gl)`.

---

## Priority summary

| Priority | Groups | Theme |
|----------|--------|-------|
| High | A, R | API correctness (raw handles, portability) |
| High | B, C | Typed upload helpers, named constructors |
| High | W1, W2 | Build hygiene |
| Medium | D, E | RAII guards, shared base class |
| Medium | F, G | State queries, existence checks |
| Medium | H, I | Missing uniform and draw call variants |
| Medium | U | Testing |
| Low | J, K, L, M, N, O, P | Full meta-gl coverage |
| Low | Q, S, T | Higher-level helpers, config, registry |
| Low | V, W3 | More examples, install export |
