#pragma once

#if defined(_MSC_VER)
    //  Microsoft
    #define MARKLAR_EXPORT __declspec(dllexport)
    #define MARKLAR_IMPORT __declspec(dllimport)
    #define MARKLAR_UNUSED
    #define MARKLAR_WARN_UNUSED_RESULT _Check_return_
#elif defined(__GNUC__)
    //  GCC
    #define MARKLAR_EXPORT __attribute__((visibility("default")))
    #define MARKLAR_IMPORT
    #define MARKLAR_UNUSED __attribute__((unused))
    #define MARKLAR_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#elif defined(__clang__)
    //  Clang
    #define MARKLAR_EXPORT __attribute__((visibility("default")))
    #define MARKLAR_IMPORT
    #define MARKLAR_UNUSED __attribute__((unused))
    #define MARKLAR_WARN_UNUSED_RESULT __attribute__((warn_unused_result))
#else
    #define MARKLAR_EXPORT
    #define MARKLAR_IMPORT
    #define MARKLAR_UNUSED
    #define MARKLAR_WARN_UNUSED_RESULT

    #pragma warning Unknown dynamic link import/export semantics.
#endif
