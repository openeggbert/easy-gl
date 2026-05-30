# easy-gl TODO — full meta-gl coverage

This file tracks everything in meta-gl that easy-gl does not yet expose.
Each item maps to one or more `metagl::gl*` functions from `Functions.hpp`.

---

## 1. State getters — `Device` extensions

Almost all `glGet*` functions are missing. Add query methods to `Device`
(or a separate `DeviceQuery` helper) covering:

### 1a. Generic scalar getters
- `glGetBooleanv` → `get_boolean(GetParameter) -> bool`
- `glGetFloatv` → `get_float(GetParameter) -> float`
- `glGetInteger64v` → `get_integer64(GetParameter) -> int64_t`
- `glGetIntegeri_v` → `get_integeri(GetParameter, index) -> int`
- `glGetInteger64i_v` → `get_integer64i(GetParameter, index) -> int64_t`
- `glGetBooleani_v` → `get_booleani(GetParameter, index) -> bool`

### 1b. Texture parameter getters — `Texture`
- `glGetTexParameterfv` → `get_parameter_fv(target, pname, out)`
- `glGetTexParameteriv` → `get_parameter_iv(target, pname, out)`
- `glGetTexParameterIiv` → `get_parameter_iiv(target, pname, out)`
- `glGetTexParameterIuiv` → `get_parameter_iuiv(target, pname, out)`
- `glGetTexLevelParameteriv` → `get_level_parameter(target, level, pname) -> int`
- `glGetTexLevelParameterfv` → `get_level_parameter(target, level, pname) -> float`

### 1c. Buffer parameter getters — `Buffer`
- `glGetBufferParameteriv` → `get_parameter(target, pname) -> int`
- `glGetBufferParameteri64v` → `get_parameter64(target, pname) -> int64_t`
- `glGetBufferPointerv` → `get_pointer(target) -> void*`

### 1d. Vertex attribute getters — `VertexArray`
- `glGetVertexAttribfv` → `get_attribute_fv(index, pname, out)`
- `glGetVertexAttribiv` → `get_attribute_iv(index, pname, out)`
- `glGetVertexAttribIiv` → `get_attribute_iiv(index, pname, out)`
- `glGetVertexAttribIuiv` → `get_attribute_iuiv(index, pname, out)`
- `glGetVertexAttribPointerv` → `get_attribute_pointer(index) -> void*`

### 1e. Uniform value getters — `Program`
- `glGetUniformfv` → `get_uniform_fv(location, out)`
- `glGetUniformiv` → `get_uniform_iv(location, out)`
- `glGetUniformuiv` → `get_uniform_uiv(location, out)`
- `glGetnUniformfv` → `get_uniform_fv_robust(location, buf_size, out)`
- `glGetnUniformiv` → `get_uniform_iv_robust(location, buf_size, out)`
- `glGetnUniformuiv` → `get_uniform_uiv_robust(location, buf_size, out)`

### 1f. Program introspection — `Program`
- `glGetActiveAttrib` → `get_active_attrib(index, out name/type/size)`
- `glGetActiveUniform` → `get_active_uniform(index, out name/type/size)`
- `glGetActiveUniformsiv` → `get_active_uniforms(indices, pname, out)`
- `glGetUniformIndices` → `get_uniform_indices(names) -> vector<uint>`
- `glGetActiveUniformBlockiv` → `get_uniform_block(block_index, pname, out)`
- `glGetActiveUniformBlockName` → `get_uniform_block_name(block_index) -> string`
- `glGetFragDataLocation` → `frag_data_location(name) -> int`
- `glGetAttachedShaders` → `get_attached_shaders() -> vector<uint>`
- `glGetProgramInterfaceiv` → `get_interface(ProgramInterface, pname) -> int`
- `glGetProgramResourceIndex` → `get_resource_index(ProgramInterface, name) -> uint`
- `glGetProgramResourceName` → `get_resource_name(ProgramInterface, index) -> string`
- `glGetProgramResourceiv` → `get_resource(ProgramInterface, index, props, out)`
- `glGetProgramResourceLocation` → `get_resource_location(ProgramInterface, name) -> int`
- `glGetProgramBinary` → `get_binary(out format, out data)`
- `glGetTransformFeedbackVarying` → `get_transform_feedback_varying(index, out)`

### 1g. Shader introspection — `Shader`
- `glGetShaderSource` → `source() -> string`
- `glGetShaderPrecisionFormat` → `get_precision_format(ShaderType, PrecisionType, out)`

### 1h. Framebuffer getters — `Framebuffer`
- `glGetFramebufferAttachmentParameteriv` → `get_attachment_parameter(target, attachment, pname) -> int`
- `glGetFramebufferParameteriv` → `get_parameter(target, pname) -> int`

### 1i. Renderbuffer getters — `Renderbuffer`
- `glGetRenderbufferParameteriv` → `get_parameter(pname) -> int`

### 1j. Sampler getters — `Sampler`
- `glGetSamplerParameterfv` → `get_parameter_fv(pname, out)`
- `glGetSamplerParameteriv` → `get_parameter_iv(pname, out)`
- `glGetSamplerParameterIiv` → `get_parameter_iiv(pname, out)`
- `glGetSamplerParameterIuiv` → `get_parameter_iuiv(pname, out)`

### 1k. Sync getter — `Sync`
- `glGetSynciv` → `get_parameter(SyncParameter) -> int`

### 1l. Query getter — `Query`
- `glGetQueryiv` → `Device::get_query_parameter(QueryTarget, QueryParameter) -> int`

### 1m. Debug getters — `Device`
- `glGetDebugMessageLog` → `get_debug_message_log(count, out sources/types/ids/severities/messages)`
- `glGetObjectLabel` → `get_object_label(DebugObjectLabel, name) -> string`
- `glGetObjectPtrLabel` → `get_object_ptr_label(ptr) -> string`
- `glGetPointerv` → `get_pointer(GetPointerParameter) -> void*`

### 1n. Miscellaneous getters — `Device`
- `glGetInternalformativ` → `get_internal_format(InternalFormatTarget, InternalFormat, pname, count, out)`
- `glGetMultisamplefv` → `get_multisample(MultisampleParameter, index, out)`

---

## 2. Existence checks (`glIs*`) — `Device` or individual classes

Each resource class could expose `is_valid_handle()` calling the GL validator,
or add static helpers to `Device`:

- `glIsBuffer` → `Buffer::is_valid_gl_object()`
- `glIsEnabled` → `Device::is_enabled(Capability) -> bool`
- `glIsEnabledi` → `Device::is_enabled(Capability, index) -> bool`
- `glIsFramebuffer` → `Framebuffer::is_valid_gl_object()`
- `glIsProgram` → `Program::is_valid_gl_object()`
- `glIsProgramPipeline` → `ProgramPipeline::is_valid_gl_object()`
- `glIsQuery` → `Query::is_valid_gl_object()`
- `glIsRenderbuffer` → `Renderbuffer::is_valid_gl_object()`
- `glIsSampler` → `Sampler::is_valid_gl_object()`
- `glIsShader` → `Shader::is_valid_gl_object()`
- `glIsSync` → `Sync::is_valid_gl_object()`
- `glIsTexture` → `Texture::is_valid_gl_object()`
- `glIsTransformFeedback` → `TransformFeedback::is_valid_gl_object()`
- `glIsVertexArray` → `VertexArray::is_valid_gl_object()`

---

## 3. `glProgramUniform*` — `ProgramPipeline` / `Program`

Setting uniforms directly on a program without binding it (required for
separable shader programs used with `ProgramPipeline`).

Add `set_program_uniform(program_handle, location, ...)` overloads mirroring
the full `set_uniform` family in `Program`, or expose them as static methods:

- `glProgramUniform1f/2f/3f/4f`
- `glProgramUniform1i/2i/3i/4i`
- `glProgramUniform1ui/2ui/3ui/4ui`
- `glProgramUniform1fv/2fv/3fv/4fv`
- `glProgramUniform1iv/2iv/3iv/4iv`
- `glProgramUniform1uiv/2uiv/3uiv/4uiv`
- `glProgramUniformMatrix2fv/3fv/4fv`
- `glProgramUniformMatrix2x3fv/2x4fv/3x2fv/3x4fv/4x2fv/4x3fv`

---

## 4. `glUniform*` array variants — `Program`

Integer and unsigned-integer array uniforms are missing. `set_uniform_fv()`
exists for floats but the equivalents for `int` and `uint` arrays are absent:

- `glUniform1iv/2iv/3iv/4iv` → `set_uniform_iv(location, span<int>, components)`
- `glUniform1uiv/2uiv/3uiv/4uiv` → `set_uniform_uiv(location, span<uint>, components)`

---

## 5. Advanced draw calls — `Device`

- `glDrawRangeElements` → `draw_range_elements(primitive, start, end, count, type, indices)`
- `glDrawRangeElementsBaseVertex` → `draw_range_elements_base_vertex(...)`
- `glDrawElementsBaseVertex` → `draw_elements_base_vertex(primitive, count, type, indices, base_vertex)`
- `glDrawElementsInstancedBaseVertex` → `draw_elements_instanced_base_vertex(...)`
- `glDrawArraysIndirect` → `draw_arrays_indirect(primitive, indirect)`
- `glDrawElementsIndirect` → `draw_elements_indirect(primitive, type, indirect)`
- `glDispatchComputeIndirect` → `dispatch_compute_indirect(indirect)`

---

## 6. Texture operations — `Texture`

- `glCopyTexImage2D` → `copy_image_2d(target, level, InternalFormat, x, y, w, h)`
- `glCopyTexSubImage2D` → `copy_sub_image_2d(target, level, xoff, yoff, x, y, w, h)`
- `glCopyTexSubImage3D` → `copy_sub_image_3d(target, level, xoff, yoff, zoff, x, y, w, h)`
- `glCopyImageSubData` → static `copy_image_sub_data(src, srcTarget, ..., dst, dstTarget, ...)`
- `glTexBuffer` → `set_buffer(target, InternalFormat, buffer_handle)`
- `glTexBufferRange` → `set_buffer_range(target, InternalFormat, buffer_handle, offset, size)`
- `glTexStorage3DMultisample` → `set_storage_3d_multisample(target, samples, InternalFormat, w, h, d, fixed)`
- `glTexParameterfv` → `set_parameter_fv(target, TextureParameter, const float*)`
- `glTexParameteriv` → `set_parameter_iv(target, TextureParameter, const int*)`
- `glTexParameterIiv` → `set_parameter_iiv(target, TextureParameter, const int*)`
- `glTexParameterIuiv` → `set_parameter_iuiv(target, TextureParameter, const uint*)`
- `glCompressedTexSubImage2D` → `set_compressed_sub_image_2d(target, level, x, y, w, h, format, size, data)`
- `glCompressedTexSubImage3D` → `set_compressed_sub_image_3d(...)`

---

## 7. Per-draw-buffer indexed state — `Device` (ES 3.2+)

Functions that set blend/color state independently per draw buffer index:

- `glEnablei` → `set_enabled(Capability, index, bool)` or `enable(Capability, index)`
- `glDisablei`
- `glColorMaski` → `set_color_mask(index, r, g, b, a)`
- `glBlendFunci` → `set_blend_func(index, sfactor, dfactor)`
- `glBlendFuncSeparatei` → `set_blend_func_separate(index, srcRGB, dstRGB, srcAlpha, dstAlpha)`
- `glBlendEquationi` → `set_blend_equation(index, BlendEquation)`
- `glBlendEquationSeparatei` → `set_blend_equation_separate(index, modeRGB, modeAlpha)`

---

## 8. Vertex format separation (ES 3.1+) — `VertexArray`

Alternative vertex attribute API that separates format definition from
buffer binding, allowing buffer rebinding without respecifying the format:

- `glVertexAttribFormat` → `set_attribute_format(attrib_index, size, DataType, normalized, relative_offset)`
- `glVertexAttribIFormat` → `set_attribute_i_format(attrib_index, size, DataType, relative_offset)`
- `glVertexAttribBinding` → `set_attribute_binding(attrib_index, binding_index)`
- `glBindVertexBuffer` → `bind_vertex_buffer(binding_index, buffer, offset, stride)`
- `glVertexBindingDivisor` → `set_binding_divisor(binding_index, divisor)`

---

## 9. Constant vertex attribute values — `VertexArray`

Set a constant value for a vertex attribute when the attribute array is disabled:

- `glVertexAttrib1f/2f/3f/4f` → `set_attrib_constant(index, ...floats...)`
- `glVertexAttrib1fv/2fv/3fv/4fv` → `set_attrib_constant(index, const float*)`
- `glVertexAttribI4i` → `set_attrib_constant(index, int, int, int, int)`
- `glVertexAttribI4ui` → `set_attrib_constant(index, uint, uint, uint, uint)`
- `glVertexAttribI4iv` → `set_attrib_constant(index, const int*)`
- `glVertexAttribI4uiv` → `set_attrib_constant(index, const uint*)`

---

## 10. Debug message API — `Device`

- `glDebugMessageCallback` → `set_debug_callback(GLDEBUGPROC, user_param)`
- `glDebugMessageControl` → `set_debug_message_control(DebugSource, DebugType, DebugSeverity, ids, enabled)`
- `glDebugMessageInsert` → `insert_debug_message(DebugSource, DebugType, id, DebugSeverity, message)`
- `glGetDebugMessageLog` → see section 1m

---

## 11. Sampler vector parameters — `Sampler`

- `glSamplerParameterfv` → `set_parameter_fv(TextureParameter, const float*)`
- `glSamplerParameteriv` → `set_parameter_iv(TextureParameter, const int*)`
- `glSamplerParameterIiv` → `set_parameter_iiv(TextureParameter, const int*)`
- `glSamplerParameterIuiv` → `set_parameter_iuiv(TextureParameter, const uint*)`

---

## 12. Framebuffer — additional operations

- `glFramebufferTexture` → `attach_texture(target, attachment, texture, level)` — layered attachment for geometry shaders (ES 3.2+)
- `glFramebufferParameteri` → `set_parameter(target, FramebufferDefaultParameter, value)` — attachmentless framebuffers (ES 3.1+)
- `glInvalidateSubFramebuffer` → `invalidate_sub(target, attachments, x, y, w, h)`

---

## 13. Program binary and shader binary — `Program` / `Shader`

- `glProgramBinary` → `Program::load_binary(ProgramBinaryFormat, data, length)`
- `glProgramParameteri` → `Program::set_parameter(ProgramParameter, value)` — e.g. set separable flag before linking
- `glCreateShaderProgramv` → `Program::create_separable(ShaderType, sources)` static factory
- `glShaderBinary` → `Shader::load_binary(ShaderBinaryFormat, data, length)`
- `glReleaseShaderCompiler` → `Device::release_shader_compiler()`

---

## 14. Miscellaneous ES 3.1+ / ES 3.2+ — `Device`

- `glSampleMaski` → `set_sample_mask(mask_number, mask)` — per-word MSAA sample mask (ES 3.1+)
- `glMinSampleShading` → `set_min_sample_shading(value)` — per-sample shading fraction (ES 3.2+)
- `glBlendBarrier` → `blend_barrier()` — ensures advanced blend visibility (ES 3.2+)
- `glPrimitiveBoundingBox` → `set_primitive_bounding_box(minX, minY, minZ, minW, maxX, maxY, maxZ, maxW)` — tile-based GPU optimization hint (ES 3.2+)
- `glReadnPixels` → `read_pixels_robust(x, y, w, h, PixelFormat, PixelType, buf_size, pixels)` — robustness variant of `read_pixels`
- `glObjectPtrLabel` → `object_ptr_label(ptr, label)` — debug label for sync objects

---

## Coverage summary

| Area | meta-gl functions | easy-gl covers | Missing |
|------|:-----------------:|:--------------:|:-------:|
| State getters | 37 | 7 | **30** |
| `glIs*` checks | 14 | 0 | **14** |
| `glProgramUniform*` | 45 | 0 | **45** |
| `glUniform*` array (int/uint) | 8 | 0 | **8** |
| Advanced draw calls | 7 | 0 | **7** |
| Texture operations | 11 | 0 | **11** |
| Per-buffer indexed state | 7 | 0 | **7** |
| Vertex format separation | 5 | 0 | **5** |
| Constant vertex attrib values | 15 | 0 | **15** |
| Debug message API | 4 | 0 | **4** |
| Sampler vector params | 4 | 0 | **4** |
| Framebuffer extras | 3 | 0 | **3** |
| Program/shader binary | 5 | 0 | **5** |
| Misc ES 3.1+/3.2+ | 6 | 0 | **6** |
| **Total** | **~180** | **~165** | **~165** |
