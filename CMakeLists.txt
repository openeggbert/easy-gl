cmake_minimum_required(VERSION 3.23)

project(easy-gl
    VERSION 0.1.0
    DESCRIPTION "Modern C++ OOP wrapper over OpenGL and OpenGLES"
    LANGUAGES CXX)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

include(EasyGlOptions)
include(EasyGlPlatform)
include(EasyGlWarnings)

add_subdirectory(../meta-gl meta-gl)

add_library(easy-gl)
add_library(easy-gl::easy-gl ALIAS easy-gl)

set_target_properties(easy-gl PROPERTIES
    CXX_STANDARD 23
    CXX_STANDARD_REQUIRED YES
    CXX_EXTENSIONS NO
    VERSION ${PROJECT_VERSION}
    SOVERSION ${PROJECT_VERSION_MAJOR})

target_sources(easy-gl
    PRIVATE
        src/GenerationTracked.cpp
        src/Config.cpp
        src/ContextInfo.cpp
        src/Capabilities.cpp
        src/Device.cpp
        src/Buffer.cpp
        src/Framebuffer.cpp
        src/ProgramPipeline.cpp
        src/Program.cpp
        src/Query.cpp
        src/Renderbuffer.cpp
        src/Sampler.cpp
        src/Shader.cpp
        src/Sync.cpp
        src/Texture.cpp
        src/TransformFeedback.cpp
        src/VertexArray.cpp
        src/ResourceRegistry.cpp
        src/UniformCache.cpp
        src/ScopedDebugGroup.cpp
        src/ResourceRegistration.cpp
)

target_sources(easy-gl
    PUBLIC
    FILE_SET HEADERS
    BASE_DIRS include
    FILES
        include/easygl/easygl.hpp
        include/easygl/Config.hpp
        include/easygl/Export.hpp
        include/easygl/Version.hpp
        include/easygl/Exception.hpp
        include/easygl/Types.hpp
        include/easygl/ContextInfo.hpp
        include/easygl/Capabilities.hpp
        include/easygl/Feature.hpp
        include/easygl/Device.hpp
        include/easygl/Buffer.hpp
        include/easygl/Framebuffer.hpp
        include/easygl/ProgramPipeline.hpp
        include/easygl/Program.hpp
        include/easygl/Query.hpp
        include/easygl/Renderbuffer.hpp
        include/easygl/Sampler.hpp
        include/easygl/Shader.hpp
        include/easygl/Sync.hpp
        include/easygl/Texture.hpp
        include/easygl/TransformFeedback.hpp
        include/easygl/VertexArray.hpp
        include/easygl/detail/GenerationTracked.hpp
        include/easygl/detail/NonCopyable.hpp
        include/easygl/RecoverableResource.hpp
        include/easygl/ResourceRegistry.hpp
        include/easygl/ScopedBind.hpp
        include/easygl/ResourceRegistration.hpp
        include/easygl/ScopedDebugGroup.hpp
        include/easygl/UniformCache.hpp
)

target_link_libraries(easy-gl PUBLIC meta-gl::meta-gl)

target_compile_features(easy-gl PUBLIC cxx_std_23)

easygl_apply_warnings(easy-gl)
install(TARGETS easy-gl
        EXPORT easy-gl-targets
        FILE_SET HEADERS DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)

if(EASYGL_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()

if(EASYGL_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
