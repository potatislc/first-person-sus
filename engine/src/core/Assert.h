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

inline bool debugAssert(const bool condition, const std::string& message) {
    if (!condition) {
        LOG_ERR(message);
        debugBreak();
    }

    return condition;
}

#define ASSERT(condition) debugAssert(condition)
#define ASSERT_MSG(condition, message) debugAssert(condition, message)

#endif
