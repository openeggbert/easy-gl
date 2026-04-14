#pragma once

#if defined(_WIN32) && defined(EASYGL_SHARED)
    #if defined(EASYGL_BUILDING_LIBRARY)
        #define EASYGL_API __declspec(dllexport)
    #else
        #define EASYGL_API __declspec(dllimport)
    #endif
#else
    #define EASYGL_API
#endif
