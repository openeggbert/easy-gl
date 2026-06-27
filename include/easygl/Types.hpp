#pragma once

#include <cstdint>
#include <metagl/metagl.hpp>

namespace easygl
{
    using u8  = std::uint8_t;
    using u16 = std::uint16_t;
    using u32 = std::uint32_t;
    using u64 = std::uint64_t;

    using i8  = std::int8_t;
    using i16 = std::int16_t;
    using i32 = std::int32_t;
    using i64 = std::int64_t;

    using GLGetProcAddressFn = metagl::GlGetProcAddressFn;

    // Scalar / data types
    using DataType             = metagl::DataType;
    using ErrorCode            = metagl::ErrorCode;
    using GetParameter         = metagl::GetParameter;

    // Buffer
    using BufferTarget         = metagl::BufferTarget;
    using BufferUsage          = metagl::BufferUsage;
    using MapBufferAccessMask  = metagl::MapBufferAccessMask;

    // Vertex arrays
    using VertexAttribParameter = metagl::VertexAttribParameter;

    // Textures
    using TextureTarget        = metagl::TextureTarget;
    using TextureUnit          = metagl::TextureUnit;
    using TextureParameter     = metagl::TextureParameter;
    using SamplerParameter     = metagl::SamplerParameter;
    using TextureMinFilter     = metagl::TextureMinFilter;
    using TextureMagFilter     = metagl::TextureMagFilter;
    using BlitFilter           = metagl::BlitFilter;
    using TextureWrapMode      = metagl::TextureWrapMode;
    using TextureCompareMode   = metagl::TextureCompareMode;
    using TextureSwizzle       = metagl::TextureSwizzle;
    using InternalFormat       = metagl::InternalFormat;
    using CompressedInternalFormat = metagl::CompressedInternalFormat;
    using PixelFormat          = metagl::PixelFormat;
    using PixelType            = metagl::PixelType;
    using PixelStoreParam      = metagl::PixelStoreParam;
    using ImageAccess          = metagl::ImageAccess;

    // Shaders / Programs
    using ShaderType           = metagl::ShaderType;
    using ShaderStageMask      = metagl::ShaderStageMask;

    // Framebuffers / Renderbuffers
    using FramebufferTarget         = metagl::FramebufferTarget;
    using FramebufferAttachment     = metagl::FramebufferAttachment;
    using FramebufferStatus         = metagl::FramebufferStatus;
    using RenderbufferTarget        = metagl::RenderbufferTarget;

    // Render state
    using PrimitiveType        = metagl::PrimitiveType;
    using BlendFactor          = metagl::BlendFactor;
    using BlendEquation        = metagl::BlendEquation;
    using CompareFunc          = metagl::CompareFunc;
    using CullFace             = metagl::CullFace;
    using FrontFace            = metagl::FrontFace;
    using StencilOp            = metagl::StencilOp;
    using Capability           = metagl::Capability;
    using ClearBufferBit       = metagl::ClearBufferBit;
    using ClearBuffer          = metagl::ClearBuffer;
    using DrawBuffer           = metagl::DrawBuffer;
    using ReadBuffer           = metagl::ReadBuffer;
    using HintTarget           = metagl::HintTarget;
    using HintMode             = metagl::HintMode;

    // Transform feedback
    using TransformFeedbackTarget     = metagl::TransformFeedbackTarget;
    using TransformFeedbackBufferMode = metagl::TransformFeedbackBufferMode;

    // Queries
    using QueryTarget          = metagl::QueryTarget;

    // Sync
    using SyncCondition        = metagl::SyncCondition;
    using SyncFlushMask        = metagl::SyncFlushMask;
    using SyncWaitResult       = metagl::SyncWaitResult;

    // Compute
    using MemoryBarrierMask    = metagl::MemoryBarrierMask;

    // Tessellation
    using TessellationParameter = metagl::TessellationParameter;

    // Debug
    using DebugSource          = metagl::DebugSource;
    using DebugType            = metagl::DebugType;
    using DebugSeverity        = metagl::DebugSeverity;
    using DebugObjectLabel     = metagl::DebugObjectLabel;

    // Robustness
    using GraphicsResetStatus  = metagl::GraphicsResetStatus;
}
