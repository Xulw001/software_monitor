/**
 * @file platform.h
 * @author xulw (nevermore.xulw@hotmail.com)
 * @brief Platform-specific macros and utilities for cross-platform support
 * @version 0.2
 * @date 2026-02-10
 *
 * @copyright Copyright (c) 2026
 */
#ifndef SQL_PLATFORM_H
#define SQL_PLATFORM_H

#ifdef _WIN32
#define EXPORT_API __declspec(dllexport)

#ifdef __cplusplus
#define EXPORT_C extern "C" EXPORT_API
#else
#define EXPORT_C EXPORT_API
#endif

#else
#if __GNUC__ >= 4
#define EXPORT_API __attribute__((visibility("default")))
#else
#define EXPORT_API
#endif

#ifdef __cplusplus
#define EXPORT_C extern "C" EXPORT_API
#else
#define EXPORT_C EXPORT_API
#endif
#endif

/**
 * @brief Platform-specific dynamic library loading macros
 * @details Defines macros for loading and using dynamic libraries across
 * platforms
 */
#ifdef _WIN32
#include <windows.h>
/**
 * @brief Windows dynamic library handle type
 */
#define DL_HANDLE HMODULE
/**
 * @brief Windows dynamic library open function
 */
#define DL_OPEN(path) LoadLibraryA(path)
/**
 * @brief Windows dynamic library symbol lookup function
 */
#define DL_SYM(handle, name) GetProcAddress(handle, name)
/**
 * @brief Windows dynamic library close function
 */
#define DL_CLOSE(handle) FreeLibrary(handle)
/**
 * @brief Windows dynamic library file suffix
 */
#define SUFFIX ".dll"
#else
#include <dlfcn.h>
/**
 * @brief POSIX dynamic library handle type
 */
#define DL_HANDLE void*
/**
 * @brief POSIX dynamic library open function
 */
#define DL_OPEN(path) dlopen(path, RTLD_LAZY)
/**
 * @brief POSIX dynamic library symbol lookup function
 */
#define DL_SYM(handle, name) dlsym(handle, name)
/**
 * @brief POSIX dynamic library close function
 */
#define DL_CLOSE(handle) dlclose(handle)
/**
 * @brief POSIX dynamic library file suffix
 */
#define SUFFIX ".so"
#endif

#endif