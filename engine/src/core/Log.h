#pragma once

#if defined(_MSC_VER)
#define debugBreak() (__debugbreak())
#elif defined(__clang__) || defined(__GNUC__)
#define debugBreak() (raise(SIGTRAP))
#else
#define debugBreak() (assert(false))
#endif

#ifdef NDEBUG
#define LOG(message) ()
#define LOG_ERR(message) ()

#else
#include <iostream>

#define LOG(message) (std::cout << message)
#define LOG_ERR(message) (std::cerr << message)

#endif
