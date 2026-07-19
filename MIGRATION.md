# easy-gl Migration Guide

This document lists every breaking API change planned for the next release and
shows exactly how to update call sites.  All fixes are mechanical one-liners —
there is no redesign required on the caller side.

---

## Breaking change summary

| # | What changed | Affected API | Effort to fix |
|---|-------------|--------------|---------------|
| 1 | `Framebuffer::attach_texture_2d` now takes `const Texture&` | `Framebuffer` | Remove `.native_handle()` |
| 2 | `Framebuffer::attach_texture_layer` now takes `const Texture&` | `Framebuffer` | Remove `.native_handle()` |
| 3 | `Framebuffer::attach_renderbuffer` now takes `const Renderbuffer&` | `Framebuffer` | Remove `.native_handle()` |
| 4 | `ProgramPipeline::use_stages` now takes `const Program&` | `ProgramPipeline` | Remove `.native_handle()` |
| 5 | `ProgramPipeline::set_active_shader_program` now takes `const Program&` | `ProgramPipeline` | Remove `.native_handle()` |
| 6 | `TransformFeedback::set_varyings` now takes `const Program&` | `TransformFeedback` | Remove `.native_handle()` |
| 7 | `Program::uniform_block_index` returns `std::optional<unsigned int>` | `Program` | Change the if-check |
| 8 | `Device::clear()` no longer disables `ScissorTest` | `Device` | Add explicit disable if needed |
| 9 | `Sync::native_handle()` returns `GLsync` instead of `__GLsync*` | `Sync` | Update stored type |

---

## Detailed migration instructions

---

### 1 — `Framebuffer::attach_texture_2d`

**Old signature:**
```cpp
void attach_texture_2d(FramebufferTarget target, FramebufferAttachment attachment,
                       TextureTarget tex_target, unsigned int texture, int level);
```

**New signature:**
```cpp
void attach_texture_2d(FramebufferTarget target, FramebufferAttachment attachment,
                       TextureTarget tex_target, const Texture& texture, int level);
```

**Before:**
```cpp
fb.attach_texture_2d(FramebufferTarget::Framebuffer,
                     FramebufferAttachment::Color0,
                     TextureTarget::Texture2D,
                     colorTex.native_handle(), 0);
```

**After:**
```cpp
fb.attach_texture_2d(FramebufferTarget::Framebuffer,
                     FramebufferAttachment::Color0,
                     TextureTarget::Texture2D,
                     colorTex, 0);
```

**Fix:** remove `.native_handle()`.

---

### 2 — `Framebuffer::attach_texture_layer`

Same pattern as change 1.

**Before:**
```cpp
fb.attach_texture_layer(target, attachment, arrayTex.native_handle(), level, layer);
```

**After:**
```cpp
fb.attach_texture_layer(target, attachment, arrayTex, level, layer);
```

---

### 3 — `Framebuffer::attach_renderbuffer`

**Old signature:**
```cpp
void attach_renderbuffer(FramebufferTarget target, FramebufferAttachment attachment,
                         unsigned int renderbuffer);
```

**New signature:**
```cpp
void attach_renderbuffer(FramebufferTarget target, FramebufferAttachment attachment,
                         const Renderbuffer& renderbuffer);
```

**Before:**
```cpp
fb.attach_renderbuffer(FramebufferTarget::Framebuffer,
                       FramebufferAttachment::Depth,
                       depthRb.native_handle());
```

**After:**
```cpp
fb.attach_renderbuffer(FramebufferTarget::Framebuffer,
                       FramebufferAttachment::Depth,
                       depthRb);
```

---

### 4 — `ProgramPipeline::use_stages`

**Old signature:**
```cpp
void use_stages(ShaderStageMask stages, unsigned int program);
```

**New signature:**
```cpp
void use_stages(ShaderStageMask stages, const Program& program);
```

**Before:**
```cpp
pipeline.use_stages(ShaderStageMask::Vertex, vertProg.native_handle());
```

**After:**
```cpp
pipeline.use_stages(ShaderStageMask::Vertex, vertProg);
```

---

### 5 — `ProgramPipeline::set_active_shader_program`

**Old signature:**
```cpp
void set_active_shader_program(unsigned int program);
```

**New signature:**
```cpp
void set_active_shader_program(const Program& program);
```

**Before:**
```cpp
pipeline.set_active_shader_program(myProg.native_handle());
```

**After:**
```cpp
pipeline.set_active_shader_program(myProg);
```

---

### 6 — `TransformFeedback::set_varyings`

**Old signature:**
```cpp
static void set_varyings(unsigned int program,
                         std::span<const char* const> varyings,
                         TransformFeedbackBufferMode buffer_mode);
```

**New signature:**
```cpp
static void set_varyings(const Program& program,
                         std::span<const char* const> varyings,
                         TransformFeedbackBufferMode buffer_mode);
```

**Before:**
```cpp
TransformFeedback::set_varyings(myProg.native_handle(), names, mode);
```

**After:**
```cpp
TransformFeedback::set_varyings(myProg, names, mode);
```

---

### 7 — `Program::uniform_block_index` return type

**Old return type:** `unsigned int` — returns the raw GL sentinel `0xFFFFFFFF`
(`GL_INVALID_INDEX`) when the block is not found.

**New return type:** `std::optional<unsigned int>` — returns `std::nullopt` when
not found.

**Before:**
```cpp
unsigned int blockIdx = program.uniform_block_index("Matrices");
if (blockIdx != 0xFFFFFFFFu)          // or != GL_INVALID_INDEX
{
    program.set_uniform_block_binding(blockIdx, 0);
}
```

**After:**
```cpp
auto blockIdx = program.uniform_block_index("Matrices");
if (blockIdx.has_value())
{
    program.set_uniform_block_binding(*blockIdx, 0);
}
```

Or with `if` initializer (C++17):
```cpp
if (auto blockIdx = program.uniform_block_index("Matrices"))
{
    program.set_uniform_block_binding(*blockIdx, 0);
}
```

---

### 8 — `Device::clear()` no longer silently disables `ScissorTest`

Previously `Device::clear(flags)` called `glDisable(GL_SCISSOR_TEST)` before
issuing `glClear`.  This was an undocumented side effect.

**If you relied on this behaviour** (i.e. you call `clear()` to also reset
scissor state), add the explicit call before `clear()`:

**Before (worked by accident):**
```cpp
device.set_scissor(x, y, w, h);
device.set_scissor_test_enabled(true);
// ... render pass ...
device.clear(ClearFlags::Color | ClearFlags::Depth);  // secretly disabled scissor
```

**After (explicit):**
```cpp
device.set_scissor(x, y, w, h);
device.set_scissor_test_enabled(true);
// ... render pass ...
device.set_scissor_test_enabled(false);               // explicit
device.clear(ClearFlags::Color | ClearFlags::Depth);
```

**If you never used `ScissorTest`** (most users), no change is needed.

---

### 9 — `Sync::native_handle()` return type

**Old return type:** `__GLsync*` (a GCC/glibc implementation detail)

**New return type:** `GLsync` (the standard typedef, defined as `void*` on most
platforms)

**Before:**
```cpp
__GLsync* s = sync.native_handle();
```

**After:**
```cpp
GLsync s = sync.native_handle();
```

This change only affects code that stores or passes the raw sync handle directly.
Code that only passes the result straight to a C API taking `GLsync` compiles
without changes.

---

## Scope of impact

The most commonly used classes (`Device`, `Buffer`, `Texture`, `Program`,
`VertexArray`, `Shader`) have **no breaking changes** — only additive new methods
are planned for them.

The breaking changes touch only:

- `Framebuffer` (changes 1–3) — used for render-to-texture
- `ProgramPipeline` (changes 4–5) — rarely used, requires separable shaders
- `TransformFeedback` (change 6) — rarely used
- `Program::uniform_block_index` (change 7) — used with UBOs
- `Device::clear` behaviour (change 8) — only relevant when `ScissorTest` is active
- `Sync` raw handle type (change 9) — only relevant when using fence sync objects

A typical application that renders with `Device` + `Buffer` + `Texture` +
`Program` + `VertexArray` requires **zero changes**.
