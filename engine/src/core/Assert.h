#pragma once

#ifdef NDEBUG
#define CORE_ASSERT(condition) ()
#define CORE_ASSERT_MSG(condition, message) ()
#else
#include "Log.h"

inline bool debugAssert(const bool condition) {
    if (!condition) {
        debugBreak();
    }

    return condition;
}

#define ASSERT(condition) debugAssert(condition)
#define ASSERT_MSG(condition, message) \
    do { \
        if (!(condition)) { \
        LOG_ERR("Assertion failed: " << message); \
        debugAssert(condition); \
        } \
    } while (0)


#endif
