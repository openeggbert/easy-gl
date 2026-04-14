#pragma once

#include "easygl/Export.hpp"

namespace easygl
{
    struct EASYGL_API Config
    {
        bool throw_on_missing_feature = true;
        bool enable_debug_logging = false;
        bool validate_calls = true;
    };
}