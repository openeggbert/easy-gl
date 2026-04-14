#pragma once

namespace easygl::platform
{
    using GetProcAddressFn = void* (*)(const char* name);

    struct GlFunctions
    {
        GetProcAddressFn get_proc_address = nullptr;

        bool initialized = false;
    };

    bool initialize_gl_functions(GlFunctions& functions, GetProcAddressFn callback);
}