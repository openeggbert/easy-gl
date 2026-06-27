# NEXT.md — easy-gl handoff document

## 1. Project summary

**easy-gl** is a C++20 RAII/OOP wrapper over OpenGL / OpenGL ES, built on top of **meta-gl** (a procedural type-safe OpenGL wrapper with typed handle structs, enum classes, and `std::span` helpers).

**Main goal:** Provide safe, convenient C++ resource classes that own and release OpenGL objects automatically, while delegating all low-level calls to meta-gl.

**Current phase:** Feature completion — all planned API tasks from PLAN.md have been implemented. Remaining work is configuration extensions (S1/S2), build hygiene (W1-W3), tests (U1/U2), and examples (V1-V3).

**Key architectural decisions:**
- All resource classes (`Buffer`, `Texture`, `Program`, etc.) are non-copyable, move-only RAII.
- Base class `detail::GenerationTracked` provides `handle_`, `generation_`, `is_created()`, `native_handle()`.
- meta-gl typed handles (`BufferId{handle_}`, `TextureId{handle_}`, etc.) are wrapped at every call site.
- `UniformLocation`, `AttribLocation` are typed structs — must be wrapped explicitly.
- `FramebufferAttachment::Color0` removed; use `metagl::to_framebuffer_attachment(metagl::ColorAttachment::Color0)`.

---

## 2. Current status

**Build:** Clean — all targets build without errors or warnings.

**Tests:** Three smoke test binaries exist:
- `easy-gl-smoke-tests` — init/device tests (no GL context required)
- `easy-gl-resource-smoke-tests` — resource object creation smoke tests
- `easy-gl-context-lifecycle-tests` — ResourceRegistry context-loss/restore cycle

All tests compile and link. Runtime tests require an OpenGL context (EGL/SDL).

**Library:** `libeasy-gl.a` (static)

**Example:** `hello-triangle-sdl` — SDL2+OpenGL ES triangle demo (builds, requires display).

**What does NOT work yet:**
- No headless automated test execution (no EGL pbuffer setup in CI).
- `Config::log_callback` field does not exist yet (S1/S2 not done).
- No CMake install/package export (`find_package(easy-gl)` not supported — W3).
- `CXX_STANDARD` is set to 23 in CMake but CLAUDE.md targets C++20 (W1).

---

## 3. Recent changes

**New files added:**
- `include/easygl/ScopedBind.hpp` — RAII bind/unbind helper
- `include/easygl/ScopedDebugGroup.hpp` + `src/ScopedDebugGroup.cpp` — RAII GPU debug groups
- `include/easygl/UniformCache.hpp` + `src/UniformCache.cpp` — cached `glGetUniformLocation`
- `include/easygl/ResourceRegistration.hpp` + `src/ResourceRegistration.cpp` — RAII registry guard

**Major additions to existing files:**
- `Device`: generic state getters (`get_boolean/float/integer/integer64`), indexed blend/color/enable, advanced draw calls, debug message API, ES 3.1+/3.2+ methods
- `Program`: `set_uniform_iv/uiv`, `set_program_uniform*` (separable), `get_uniform_fv/iv/uiv`, introspection, `load_binary`, `create_separable`, `set_parameter`
- `Texture`: copy ops, buffer textures, compressed sub-image, parameter getters, set_parameter array variants
- `Buffer`: `get_parameter`, `get_parameter64`, `get_pointer`
- `VertexArray`: separate vertex format (K1), constant attribs (L1), attribute getters (F4)
- `Framebuffer`: `attach_texture`, `blit_to`, `invalidate_sub`, `set/get_parameter`, `get_attachment_parameter`
- `Shader`: `load_binary`
- `Sampler`, `Query`, `Sync`, `Renderbuffer`: parameter getters added

**Types.hpp additions:** `ProgramParameter`, `ProgramBinaryFormat`, `ShaderBinaryFormat`, `BufferParameter`, `RenderbufferParameter`, `FramebufferAttachmentParameter`, `FramebufferDefaultParameter`, `TextureLevelParameter`, `SyncParameter`

---

## 4. Current blocker / main problem

**No blocker** — the build is clean, all planned API tasks are done.

The only remaining gap before the library is fully production-ready:
- `Config::log_callback` (S1) is missing, so `enable_debug_logging = true` has nowhere to send output.
- CMake C++ standard mismatch: `CXX_STANDARD 23` vs. documented target of C++20 (W1).

---

## 5. Known bugs and limitations

- **incomplete** — `Config::enable_debug_logging = true` silently does nothing (no callback hooked up). Fix: implement S1/S2.
- **incomplete** — `Query::result_u64()` uses 32-bit `glGetQueryObjectuiv` internally (meta-gl does not expose `glGetQueryObjectui64v`). Returns values ≤ UINT32_MAX only. Suitable for frame counts; not for high-resolution GPU timers.
- **incomplete** — No headless test runner. Smoke tests compile but require an EGL/display to actually execute GL calls.
- **incomplete** — `draw_elements_base_vertex` and similar ES 3.2+ functions compile but will fail at runtime on ES 3.0 contexts (expected; no version guard).
- **incomplete** — W3: no `find_package(easy-gl)` support; downstream users must use `add_subdirectory`.
- **needs verification** — `Program::create_separable` sets `GL_PROGRAM_SEPARABLE` before linking; this is correct for ES 3.1+ but untested at runtime.

---

## 6. Architecture notes

```
OpenGL / OpenGL ES
      ↓
meta-gl          — procedural, typed, no ownership
      ↓
easy-gl          — OOP, RAII, move-only ownership
```

**Resource ownership model:**
- Every GL object class holds `handle_` (unsigned int) from `GenerationTracked`.
- Destructor calls `destroy()` which calls the appropriate `glDelete*`.
- Move constructor/assignment zero out the source handle to prevent double-delete.
- `reset_handle_no_gl()` zeros without calling GL (used after context loss).

**meta-gl integration rules:**
- Always wrap `handle_` in the typed struct at call sites: `metagl::BufferId{handle_}`.
- `UniformLocation{location}` and `AttribLocation{index}` must be explicit.
- Never call raw `glXxx()` if meta-gl already wraps it.

**Context lifecycle:**
- `ResourceRegistry` implements `metagl::ContextListener`.
- On context loss: `OnContextLost()` calls `release_gl_handle_only()` on all registered resources.
- On context restore: `OnContextRestored()` calls `recreate_gl_resource()`.
- `ResourceRegistration` (T1) provides RAII auto-register/deregister.

**Stable boundaries (do not break):**
- Public header API — downstream projects depend on it.
- `GenerationTracked` base — all resource classes inherit it.
- meta-gl is the only low-level GL dependency; no raw GL includes in easy-gl headers.

---

## 7. Useful commands

```bash
# Configure (from repo root)
cmake -B build -DEASYGL_BUILD_TESTS=ON

# Build everything
cmake --build build

# Run smoke tests (no GL context needed for init tests)
./build/tests/easy-gl-smoke-tests

# Run hello-triangle example (requires display + SDL2 + OpenGL ES)
./build/examples/integration-placeholder/hello-triangle-sdl/hello-triangle-sdl

# Check build only (no run)
cmake --build build 2>&1 | grep -E "error:|warning:"
```

---

## 8. Next smallest tasks

1. **S1 — Add `log_callback` to `Config`**
   - Files: `include/easygl/Config.hpp`
   - Add `std::function<void(std::string_view)> log_callback;` field.
   - Verify: `cmake --build build` passes cleanly.

2. **S2 — Add `auto_error_check` to `Config` + wire it in `Device`**
   - Files: `include/easygl/Config.hpp`, `src/Device.cpp`
   - When `auto_error_check = true`, call `glGetError()` after operations in debug builds and invoke `log_callback`.
   - Verify: build passes; optionally add a test.

3. **W1 — Fix CMake C++ standard**
   - File: `CMakeLists.txt`
   - Change `CXX_STANDARD 23` to `CXX_STANDARD 20`; add `CXX_STANDARD_REQUIRED ON`.
   - Verify: `cmake --build build` passes.

4. **W2 — Verify `EASYGL_BUILD_TESTS` option works cleanly**
   - File: `CMakeLists.txt`, `tests/CMakeLists.txt`
   - Ensure `cmake -B build -DEASYGL_BUILD_TESTS=OFF` builds without the test targets.
   - Verify: `cmake --build build -DEASYGL_BUILD_TESTS=OFF` produces no test binaries.

5. **W3 — Add `cmake/EasyGlInstall.cmake` package export**
   - Files: `CMakeLists.txt`, new `cmake/EasyGlInstall.cmake`
   - Add `configure_package_config_file` + `write_basic_package_version_file`.
   - Verify: `cmake --install build --prefix /tmp/easy-gl-install` and check for `easy-glConfig.cmake`.

6. **U1 — Add one real headless test using EGL pbuffer**
   - Files: new `tests/headless/HeadlessContext.hpp`, `tests/headless/BufferTest.cpp`
   - Create EGL pbuffer context; test `Buffer::create_vertex_buffer` + `is_valid_gl_object()`.
   - Verify: `./build/tests/easy-gl-headless-tests` passes.

7. **V1 — Add textured-quad example**
   - Files: new `examples/hello-textured-quad/`
   - Demonstrate `Texture::create_2d_rgba8`, `Sampler::create_linear_clamp`, `ScopedBind`.
   - Verify: example builds and links.

8. **V3 — Add UBO example**
   - Files: new `examples/hello-ubo/`
   - Demonstrate `Buffer` with `UniformBuffer`, `Program::uniform_block_index`, `set_uniform_block_binding`.
   - Verify: example builds and links.

---

## 9. Do not do yet

- Do not redesign or refactor existing resource classes — the API is stable.
- Do not add C++23-only features (modules, `std::expected` everywhere) — target is C++20.
- Do not change the meta-gl dependency version without checking easy-gl compatibility first.
- Do not add new resource classes (e.g. `Pipeline`, `AtomicCounter`) without a clear use case.
- Do not merge `develop` into `master` until S1/S2 and W1 are done.
- Do not introduce a global error-handling philosophy — use the existing `Exception` class.
- Do not mass-rename existing public API methods — downstream breakage.

---

## 10. Resume prompt

```
Read NEXT.md first. Then inspect only the files needed for the first task listed in section 8.
Do not refactor unrelated code. Make one small, verified improvement. Run:
  cmake --build build 2>&1 | grep -E "error:|warning:"
to verify the build is clean after your change. Update NEXT.md section 3 (Recent changes)
and section 2 (Current status) to reflect what you did. Then stop and report.
```
