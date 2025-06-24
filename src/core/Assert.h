#pragma once

#if defined(_MSC_VER)
#define debugBreak() (__debugbreak())
#elif defined(__clang__) || defined(__GNUC__)
#define debugBreak() (raise(SIGTRAP))
#else
#define debugBreak() (assert(false))
#endif

#include <iostream>

namespace Core {
    inline bool debugAssert(const bool condition, const std::string& message = {}) {
        if (!condition) {
            std::cerr << message;
            debugBreak();
        }

        return condition;
    }
}

#ifdef NDEBUG
#define CORE_ASSERT(condition) ()
#define CORE_ASSERT_MSG(condition, message) ()
#else
#define CORE_ASSERT(condition) Core::debugAssert(condition)
#define CORE_ASSERT_MSG(condition, message) Core::debugAssert(condition, message)
#endif
