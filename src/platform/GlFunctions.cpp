#include "GlFunctions.hpp"

namespace easygl::platform
{
    bool initialize_gl_functions(GlFunctions& functions, GetProcAddressFn callback)
    {
        if (callback == nullptr)
        {
            return false;
        }

        functions.get_proc_address = callback;
        functions.initialized = true;
        return true;
    }
}