#pragma once

#include <cstdint>

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

    /**
     * @brief A function pointer type that loads OpenGL function pointers.
     * 
     * This type is compatible with SDL_GL_GetProcAddress, glfwGetProcAddress, etc.
     */
    using GLGetProcAddressFn = void* (*)(const char* name);

    enum class DataType
    {
        Float,
        Byte,
        UnsignedByte,
        Short,
        UnsignedShort,
        Int,
        UnsignedInt
    };
}