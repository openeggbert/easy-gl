#pragma once

namespace easygl
{
    enum class Feature
    {
        VertexArrayObject,
        DebugOutput,
        ComputeShader,
        GeometryShader,
        TessellationShader,
        Texture3D,
        TextureFloat,
        TextureRG,
        SamplerObject,
        Instancing,
        UniformBufferObject,
        FramebufferObject,
        Multisampling,
        DrawBuffers,
        MapBuffer,
        DirectStateAccess,
        ClipControl,
        TextureCompression,
        AnisotropicFiltering,
        OpenGlOnlyImmediateMode,
        OpenGlOnlyPolygonMode,
        OpenGlOnlyLineWidthWide
    };
}